import React, { useContext, useState } from 'react';
import { Grid, Button, Snackbar } from '@material-ui/core/';

import { VideoEngineContext } from '../App';
import { drawCanvasFrame, clearCanvasFrame } from '../utils';

function Controls() {
	const {
		input,
		status,
		actions,
		videoEngine,
	} = useContext(VideoEngineContext);
	const { loading } = status;
	const { setDetected, setEyesInfo, setHeadTurns, setBestPrediction, setLivenessConditions, setBestShotConditions } = actions;
	const [initialization, setInitialization] = useState(false);
	const [isInitialized, setisInitialized] = useState(false);

	// A function that is called each time during detection processing
	const detectionCallback = (prediction) => {
		if (videoEngine.isInitialised) {
			setInitialization(false);
			setisInitialized(true);
		}

		if (prediction && videoEngine.inProgress) {
			setDetected(true);

			// Check turns head
			checkTurnsHead(prediction);

			// Best prediction and best shot
			const bestPrediction = videoEngine.bestPrediction;
			setBestShotConditions((prevState) => {
				const newState = {
					eyesDistanceInRequiredRange: prediction.face.eyes.eyesDistanceInRequiredRange,
					poseInRequiredRange: prediction.pose.poseInRequiredRange,
				};
				return JSON.stringify(newState) !== JSON.stringify(prevState) ? newState : prevState;
			})
			if (bestPrediction)
				setBestPrediction(bestPrediction);

			// Get eyes info
			setEyesInfo((prevState) => {
				const newState = {
					eyesIsClosed: prediction.face.eyes.isClosed,
					eyesBlinking: prediction.face.eyes.isBlinked,
				}
				return JSON.stringify(newState) !== JSON.stringify(prevState) ? newState : prevState;
			});

			// 2D Liveness
			setLivenessConditions((prevState) => {
				const newState = {
					headWasTurned: prediction.headWasTurned,
					eyesWasBlinking: prediction.face.eyes.wasBlinked,
				};
				return JSON.stringify(newState) !== JSON.stringify(prevState) ? newState : prevState;
			})

			drawCanvasFrame(prediction);
		} else {
			setDetected(false);
			clearCanvasFrame();
		}
	};

	const handlerStartDetect = async () => {
		setInitialization(true);
		await videoEngine.start(input, detectionCallback);
	};

	const handlerStopDetect = () => {
		const newBestPredict = videoEngine.stop();
		clearCanvasFrame();
		if (newBestPredict) setBestPrediction(newBestPredict);
		setDetected(false);
	};

	const handlerResetDetect = () => {
		videoEngine.reset();
		clearCanvasFrame();
		setDetected(false);
		setBestPrediction(null);
		setBestShotConditions({
			eyesDistanceInRequiredRange: false,
			poseInRequiredRange: false,
		});
		setEyesInfo({
			eyesIsClosed: false,
			eyesBlinking: false,
		});
		setLivenessConditions({
			headWasTurned: false,
			eyesWasBlinking: false,
		});
		setHeadTurns({
			up: false,
			down: false,
			left: false,
			right: false,
		});
	};

	const checkTurnsHead = (prediction) => {
		const { pose } = prediction;
		const needAngleTurn = 15;
		setHeadTurns((prev) => {
			if (!prev.up) {
				if (pose.axes.pitch > needAngleTurn) return { ...prev, up: true };
				else return prev;
			}
			if (!prev.down) {
				if (pose.axes.pitch < -needAngleTurn) return { ...prev, down: true };
				else return prev;
			}
			if (!prev.left) {
				if (pose.axes.yaw > needAngleTurn) return { ...prev, left: true };
				else return prev;
			}
			if (!prev.rigth) {
				if (pose.axes.yaw < -needAngleTurn) return { ...prev, right: true };
				else return prev;
			}
			return prev;
		});
	};

	return (
		<>
			<Snackbar
				anchorOrigin={{
					vertical: 'top',
					horizontal: 'center',
				}}
				open={initialization && !isInitialized}
				message="Initializing, plase wait...."
			/>
			<Grid container justify="center" spacing={2}>
				<Grid item>
					<Button
						disabled={loading}
						onClick={handlerStartDetect}
						variant="contained"
						color="primary">
						Start
					</Button>
				</Grid>
				<Grid item>
					<Button disabled={loading} onClick={handlerStopDetect} variant="contained">
						Stop
					</Button>
				</Grid>
				<Grid item>
					<Button
						disabled={loading}
						onClick={handlerResetDetect}
						variant="contained"
						color="secondary">
						Clear
					</Button>
				</Grid>
			</Grid>
		</>
	);
}

export default Controls;

import React, { useEffect, useState, useRef } from 'react';
import { initializationCam } from './utils';
import { Snackbar, Button } from '@material-ui/core/';
import { VideoEngine } from '@3divi/face-sdk-video-engine-js';

import Demo from './Demo';

import './App.css';

const VIDEO_ID = 'video';
const CANVAS_ID = 'canvasOutput';
export const VideoEngineContext = React.createContext();

function App() {
	const [loading, setLoading] = useState(true);
	const [error, setError] = useState('');

	const videoRef = useRef({});
	const [videoEngine, setVideoEngine] = useState(null);
	const [isDetected, setDetected] = useState(false);
	const [bestPrediction, setBestPrediction] = useState(null);

	// Conditions for getting bestshot
	const [bestShotConditions, setBestShotConditions] = useState({
		eyesDistanceInRequiredRange: false,
		poseInRequiredRange: false,
	});
	// Conditions for 2D Liveness
	const [livenessConditions, setLivenessConditions] = useState({
		headWasTurned: false,
		eyesWasBlinking: false,
	});
	// Current eyes info
	const [eyesInfo, setEyesInfo] = useState({
		eyesIsClosed: false,
		eyesBlinking: false,
	});
	// The state of rotation of the head
	const [headTurns, setHeadTurns] = useState({
		up: false,
		down: false,
		left: false,
		right: false,
	});

	useEffect(() => {
		setLoading(true);
		initDemo();
	}, []);

	const initDemo = async () => {
		try {
			setError('');
			await initializationCam(VIDEO_ID, CANVAS_ID);
			const videoEngine = new VideoEngine({});
			await videoEngine.load();
			setVideoEngine(videoEngine);
			setLoading(false);
		} catch (error) {
			setError(error.message);
		}
	};

	return (
		<VideoEngineContext.Provider
			value={{
				input: videoRef.current,
				videoEngine,
				status: {
					loading,
					isDetected,
				},
				actions: {
					setEyesInfo,
					setDetected,
					setHeadTurns,
					setBestPrediction,
					setBestShotConditions,
					setLivenessConditions,
				},
				bestPrediction,
				constants: {
					videoId: VIDEO_ID,
					canvasId: CANVAS_ID,
				},
			}}>
			<Snackbar
				anchorOrigin={{
					vertical: 'bottom',
					horizontal: 'left',
				}}
				open={Boolean(error)}
				message={error}
				action={
					<Button variant="contained" color="primary" onClick={initDemo}>
						Retry
					</Button>
				}
			/>
			<Demo
				bestShotConditions={bestShotConditions}
				livenessConditions={livenessConditions}
				headTurns={headTurns}
				videoRef={videoRef}
				eyesInfo={eyesInfo}
			/>
		</VideoEngineContext.Provider>
	);
}

export default React.memo(App);

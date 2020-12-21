import React, { useContext } from 'react';
import { clearCanvasFrame } from './utils';
import { VideoEngineContext } from './App';

import { Typography, Grid, Container, CircularProgress, Box } from '@material-ui/core/';
import ListConditions from './components/ListConditions';
import DetectionProgressTimer from './components/DetectionProgressTimer';
import Controls from './components/Controls';
import Assistant from './components/Assistant';

function Demo({ videoRef, eyesInfo, bestShotConditions, livenessConditions, headTurns }) {
	const { constants, status, actions, bestPrediction, videoEngine } = useContext(
		VideoEngineContext
	);
	const { videoId, canvasId } = constants;
	const { loading, isDetected } = status;
	const { setDetected, setBestPrediction } = actions;
	return (
		<Container maxWidth="xl">
			{isDetected && <Assistant bestshotConditions={bestShotConditions} livenessConditions={livenessConditions} headTurns={headTurns} />}
			<Grid className="wrapper" justify="center" container spacing={3}>
				<Grid item xs={12}>
					<Typography align="center" style={{ fontWeight: 600 }} variant="h5">
						3DiVi Face SDK VideoEngine JS Demo
					</Typography>
					<Typography align="center" style={{ fontWeight: 600 }} >
						This is a demo of face detection, tracking and active liveness estimation from web browser, implemented into <a href="https://github.com/3DiVi/face-sdk/tree/master/doc/en/samples/javascript/js_demo.md">3DiVi Face SDK VideoEngine JS module</a>.
					</Typography>

				</Grid>
				<Grid item xs={12}>
					<Typography align="center" style={{ fontWeight: 300, textAlign: "left" }} >
						<ul text-align="left">
							<li>Please note that the demo application doesn't support mobile web-browsers.</li>
							<li>Plug in a webcam to your computer and allow this site access to the camera.</li>
							<li>On the left side of the screen, the video stream from the webcam (Stream) is displayed.</li>
							<li>To start the Active Liveness check, press the Start button. During the processing, a progress bar and the phrase "Wait, detection is in progress ..." are displayed above the window with the video stream. A face is considered to belong to a real person if at least one blink was made during processing and the head was turned at least once.</li>
							<li>The best shot of the face is displayed to the right of the video stream. If the face is not positioned correctly, the browser will display hints during processing, for example, "Please turn your head up", "Please come closer to the camera", etc.</li>
							<li>Press the Stop button to stop the Active Liveness check.</li>
							<li>To clear the received data, press the Clear button.</li>
						</ul>
					</Typography>
				</Grid>

				<Grid align="center" item xs={12}>
					<Controls />
				</Grid>
				<Grid align="center" item xs={12}>
					<Grid
						alignItems="flex-start"
						justify="center"
						spacing={2}
						container
						style={{ minHeight: '400px' }}>
						<Grid item>
							{loading && <CircularProgress />}
							<Box hidden={loading} className="video__wrapper">
								<Typography align="left" variant="h6">
									Stream:
								</Typography>
								{isDetected && (
									<>
										<Typography align="left">
											Wait, detection is in progress...
										</Typography>
										<DetectionProgressTimer
											callback={() => {
												const newBestPredict = videoEngine.stop();
												if (newBestPredict) setBestPrediction(newBestPredict);
												setDetected(false);
												clearCanvasFrame();
											}}
											maxCount={30}
											isDetected={isDetected}
										/>
									</>
								)}
								<video ref={videoRef} id={videoId}></video>
								<canvas id={canvasId}></canvas>
							</Box>
						</Grid>
						{!loading && (
							<>
								<Grid item>
									<ListConditions
										title="Best shot:"
										conditions={bestShotConditions}
									/>
									{bestPrediction && bestPrediction.imageBase64 && (
										<img
											alt="Best shot"
											className="bestshot"
											src={`data:image/png;base64,${bestPrediction.imageBase64}`}
										/>
									)}
								</Grid>
								<Grid item>
									<ListConditions
										title="Active Liveness:"
										conditions={livenessConditions}
									/>
								</Grid>
								<Grid item>
									<ListConditions
										title="Eyes info:"
										conditions={eyesInfo}
									/>
								</Grid>
							</>
						)}
					</Grid>
				</Grid>
			</Grid>
		</Container >
	);
}

export default Demo;

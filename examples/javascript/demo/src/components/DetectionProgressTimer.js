import React, { useState, useEffect } from 'react';
import { LinearProgress } from '@material-ui/core/';

let timer;

function DetectionProgressTimer({ isDetected, maxCount, callback }) {
	const [progress, setProgress] = useState(0);

	useEffect(() => {
		if (!isDetected) {
			clearTimer();
		} else {
			let count = 0;
			timer = setInterval(() => {
				if (count < maxCount) {
					count++;
					const result = (count * 100) / maxCount;
					setProgress(result);
				} else {
					clearTimer();
				}
			}, 1000);
		}

		return () => {
			clearTimer();
		};
	}, [isDetected, maxCount]);

	useEffect(() => {
		if (progress >= 100) {
			callback && callback();
			clearTimer();
		}
	}, [progress, callback]);

	const clearTimer = () => {
		clearInterval(timer);
		setProgress(0);
	};

	return (
		<LinearProgress
			style={{
				height: '6px',
			}}
			variant="determinate"
			className="progress_bar"
			value={progress}
		/>
	);
}

export default DetectionProgressTimer;

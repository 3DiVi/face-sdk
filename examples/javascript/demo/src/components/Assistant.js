import React, { useContext, useState, useEffect } from 'react';
import { Snackbar } from '@material-ui/core/';
import { VideoEngineContext } from '../App';

const conditionsActions = {
	headWasTurned: 'Please turn your head in any direction',
	eyesWasBlinking: 'Please look at the camera and blink',
	eyesDistanceInRequiredRange: 'Please come closer to the camera',
	poseInRequiredRange: 'Please look at the camera',
};

const turnsText = {
	up: 'Please turn your head up',
	down: 'Please turn your head down',
	left: 'Please turn your head to the left',
	right: 'Please turn your head to the right',
};

function Assistant({ bestshotConditions, livenessConditions, headTurns }) {
	const { status } = useContext(VideoEngineContext);
	const { isDetected } = status;
	const [message, setMessage] = useState(() => '');

	useEffect(() => {
		const allHeadTurns = Object.values(headTurns).every((item) => item);
		const otherConditions = { ...bestshotConditions, eyesWasBlinking: livenessConditions.eyesWasBlinking };
		if (isDetected) {
			if (!allHeadTurns) checkConditions(headTurns, turnsText);
			else checkConditions(otherConditions, conditionsActions);
		}

		if (!isDetected && message) setMessage('');
		const allConditionsMet = Object.values(otherConditions).every((item) => item);
		if (allConditionsMet && allHeadTurns) setMessage('');
	}, [isDetected, JSON.stringify(bestshotConditions), JSON.stringify(livenessConditions), JSON.stringify(headTurns), message]);

	const checkConditions = (conditions, objectText) => {
		for (const key in conditions) {
			if (conditions.hasOwnProperty(key)) {
				const value = conditions[key];
				if (!value) {
					setMessage(objectText[key]);
					return;
				}
			}
		}
	};

	return (
		<Snackbar
			anchorOrigin={{
				vertical: 'top',
				horizontal: 'center',
			}}
			open={Boolean(message)}
			message={message}
		/>
	);
}

export default React.memo(Assistant);

import React from 'react';
import { FormGroup, FormControlLabel, Checkbox, Typography } from '@material-ui/core/';
import IndeterminateCheckBox from '@material-ui/icons/IndeterminateCheckBox';
import Check from '@material-ui/icons/Check';
import { withStyles } from '@material-ui/core/styles';
import { green, red } from '@material-ui/core/colors';

const conditionsData = {
	eyesWasBlinking: 'There was an eye blink',
	headWasTurned: 'The head was turned',
	poseInRequiredRange: 'Correct face position',
	eyesDistanceInRequiredRange: 'Acceptable distance from the camera',
	headUp: 'Turn head up',
	headDown: 'Turn head down',
	headLeft: 'Turn head left',
	headRight: 'Turn head right',
	eyesBlinking: 'Blinking',
	eyesIsClosed: 'Closed',
};

function ListConditions({ title, conditions }) {
	return (
		<FormGroup>
			<Typography align="left" variant="h6">
				{title}
			</Typography>
			{Object.entries(conditions).map(([key, value]) => (
				<FormControlLabel
					style={{ cursor: 'default' }}
					key={key}
					control={<GreenCheckbox checked={!value ? false : value} color="primary" name={key} />}
					label={conditionsData[key]}
				/>
			))}
		</FormGroup>
	);
}

const GreenCheckbox = withStyles({
	root: {
		cursor: 'default',
		color: red[400],
		'&$checked': {
			color: green[600],
		},
	},
	checked: {},
})((props) => (
	<Checkbox
		icon={<IndeterminateCheckBox fontSize="small" />}
		checkedIcon={<Check fontSize="small" />}
		disableRipple
		color="default"
		{...props}
	/>
));

export default React.memo(ListConditions);

const VIDEO_SIZE = document.documentElement.clientWidth * 0.35;
let ctx, canvas;

export async function initializationCam(videoId, canvasId) {
	let videoHeight, videoWidth;
	try {
		const video = await setupCamera(videoId);
		video.play();
		videoWidth = video.videoWidth;
		videoHeight = video.videoHeight;
		video.width = videoWidth;
		video.height = videoHeight;
	} catch (error) {
		throw new Error(error);
	}

	canvas = document.getElementById(canvasId);
	canvas.width = videoWidth;
	canvas.height = videoHeight;
	const canvasContainer = document.querySelector('.video__wrapper');
	canvasContainer.style = `width: ${videoWidth}px; height: ${videoHeight}px`;
	ctx = canvas.getContext('2d');
	ctx.fillStyle = 'rgba(255, 0, 0, 1)';
	ctx.strokeStyle = 'rgba(255, 0, 0, 1)';
	ctx.lineWidth = 2;
	ctx.translate(canvas.width, 0);
	ctx.scale(-1, 1);
}

async function setupCamera(videoId) {
	const video = document.getElementById(videoId);
	try {
		const stream = await navigator.mediaDevices.getUserMedia({
			audio: false,
			video: {
				facingMode: 'user',
				width: VIDEO_SIZE,
				height: VIDEO_SIZE,
			},
		});
		video.srcObject = stream;
		return new Promise((resolve) => {
			video.onloadedmetadata = () => {
				resolve(video);
			};
		});
	} catch (error) {
		throw new Error(error);
	}
}

export const clearCanvasFrame = () => {
	ctx.clearRect(0, 0, canvas.width, canvas.height);
}

export const drawCanvasFrame = (prediction, borderColor = 'rgba(255, 0, 0, 1)') => {
	ctx.strokeStyle = borderColor;
	ctx.fillStyle = borderColor;
	clearCanvasFrame();
	if (prediction) {
		let boundingBox = prediction.face.boundingBox;
		const start = boundingBox.topLeft;
		const end = boundingBox.bottomRight;
		const size = [end[0] - start[0], end[1] - start[1]];
		ctx.strokeRect(start[0], start[1], size[0], size[1]);
	}
};
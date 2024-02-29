/**
	\file csharp_demo/video_recognition_demo/src/MAssert.cs
*/

using System;

class MAssert
{
	static public void Check(bool expr, string description="")
	{
		if(!expr) 
			throw new Exception(description);
	}

}

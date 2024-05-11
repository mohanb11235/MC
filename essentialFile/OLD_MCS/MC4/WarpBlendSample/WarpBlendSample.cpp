
//
// File:           WarpBlendSample.cpp
// Description:    Sample implementation of Nvidia warp and blend API
// Target OS:      Windows 7 only
// Notes:          Now available in the public version of NVAPI
//

//Version 1.2 Changelog
//- Uses DisplayIds instead of outputIds (deprecated)
//- Removed mosaic calls to get gisplay handle. Replace with NvAPI_DISP_GetDisplayHandleFromDisplayId instead
//- Added NvAPI_GPU_GetScanoutConfigurationEx for the case when viewport rect!=display rect

//Version 1.3 Changelog
//- Added example of Warp and Blend Antialiasing - requires R352 or later
//- Added 64bit config

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <assert.h>
#include "include\nvapi.h"


int main(int argc, char **argv)
{
	NvAPI_Status error;
	NvPhysicalGpuHandle nvGPUHandles[NVAPI_MAX_PHYSICAL_GPUS];
	NvU32 gpuCount = 0;
	NvU32 gpu;
	NvU32 outputMask = 0;
	NvSBox desktopRect;
	NvSBox scanoutRect; //portion of the desktop
	NvSBox viewportRect; //the viewport which is a subregion of the scanout
	NvSBox osRect; //os coordinates of the desktop

	NV_SCANOUT_WARPING_DATA warpingData;
	NvAPI_ShortString estring;
	int maxNumVertices = 0;
	int sticky = 0;
	NV_MOSAIC_TOPO_BRIEF  topo;

	printf("App Version: 1.3\n");

	// Initialize NVAPI, get GPU handles, etc.
	error = NvAPI_Initialize();
	ZeroMemory(&nvGPUHandles, sizeof(nvGPUHandles));
	error = NvAPI_EnumPhysicalGPUs(nvGPUHandles, &gpuCount);

	// At this point we have a list of accessible physical nvidia gpus in the system.
	// Loop over all gpus

	for (gpu = 0; gpu < gpuCount; gpu++)
	{
		topo.version = NVAPI_MOSAIC_TOPO_BRIEF_VER;
		NV_MOSAIC_DISPLAY_SETTING dispSetting;
		dispSetting.version = NVAPI_MOSAIC_DISPLAY_SETTING_VER;
		NvS32 overlapX, overlapY;
		float srcLeft_1, srcTop_1, srcWidth_1, srcHeight_1;

		error = NvAPI_Mosaic_GetCurrentTopo(&topo, &dispSetting, &overlapX, &overlapY);
		if (error != NVAPI_OK)
		{
			NvAPI_GetErrorMessage(error, estring);
			printf("NvAPI_GPU_GetCurrentTopo: %s\n", estring);
		}



		NvU32 dispIdCount = 0;

		// Query the active physical display connected to each gpu.
		error = NvAPI_GPU_GetConnectedDisplayIds(nvGPUHandles[gpu], NULL, &dispIdCount, 0);
		if ((error != NVAPI_OK) || (dispIdCount == 0))
		{
			NvAPI_GetErrorMessage(error, estring);
			printf("NvAPI_GPU_GetConnectedDisplayIds: %s\n", estring);
			printf("Display count %d\n", dispIdCount);
			return error;
		}

		NV_GPU_DISPLAYIDS* dispIds = NULL;
		dispIds = new NV_GPU_DISPLAYIDS[dispIdCount];
		dispIds->version = NV_GPU_DISPLAYIDS_VER;
		error = NvAPI_GPU_GetConnectedDisplayIds(nvGPUHandles[gpu], dispIds, &dispIdCount, 0);
		if (error != NVAPI_OK)
		{
			delete[] dispIds;
			NvAPI_GetErrorMessage(error, estring);
			printf("NvAPI_GPU_GetConnectedDisplayIds: %s\n", estring);
		}


		for (int dispIndex = 0; (dispIndex < dispIdCount) && dispIds[dispIndex].isActive; dispIndex++)
		{

			ZeroMemory(&desktopRect, sizeof(desktopRect));
			ZeroMemory(&scanoutRect, sizeof(scanoutRect));
			ZeroMemory(&osRect, sizeof(osRect));
			ZeroMemory(&viewportRect, sizeof(viewportRect));
			printf("GPU %d, displayId 0x%08x\n", gpu, dispIds[dispIndex].displayId);

			// Query the desktop and scanout portion of each physical active display.
			error = NvAPI_GPU_GetScanoutConfiguration(dispIds[dispIndex].displayId, &desktopRect, &scanoutRect);
			if (error != NVAPI_OK)
			{
				NvAPI_GetErrorMessage(error, estring);
				printf("NvAPI_GPU_GetScanoutConfiguration: %s\n", estring);
			}


			//The below is optional for R331+ in cases where the viewport rect!=scanoutRect
			NV_SCANOUT_INFORMATION scanoutInformation;



			scanoutInformation.version = NV_SCANOUT_INFORMATION_VER;
			error = NvAPI_GPU_GetScanoutConfigurationEx(dispIds[dispIndex].displayId, &scanoutInformation);
			//if this new interface is supported fetch scanout data from it
			if (error == NVAPI_OK)
			{
				scanoutRect.sWidth = scanoutInformation.targetDisplayWidth;
				scanoutRect.sHeight = scanoutInformation.targetDisplayHeight;
				viewportRect = scanoutInformation.targetViewportRect;
			}
			if (topo.enabled == false)
			{
				// Extended mode
				// warp texture coordinates are defined in desktopRect coordinates
				printf("Mosaic is disabled\n");
				srcLeft_1 = (float)scanoutInformation.sourceDesktopRect.sX;
				srcTop_1 = (float)scanoutInformation.sourceDesktopRect.sY;
				srcWidth_1 = (float)scanoutInformation.sourceDesktopRect.sWidth;
				srcHeight_1 = (float)scanoutInformation.sourceDesktopRect.sHeight;
			}
			else
			{
				// Mosaic -- we only want the pixels under the physical display here
				printf("Mosaic is enabled\n");
				srcLeft_1 = (float)scanoutInformation.sourceViewportRect.sX;
				srcTop_1 = (float)scanoutInformation.sourceViewportRect.sY;
				srcWidth_1 = (float)scanoutInformation.sourceViewportRect.sWidth;
				srcHeight_1 = (float)scanoutInformation.sourceViewportRect.sHeight;
			}

			printf("srcLeft_1:%f\n", srcLeft_1);
			printf("srcTop_1:%f\n", srcTop_1);
			printf("srcWidth_1:%f\n", srcWidth_1);
			printf("srcHeight_1:%f\n", srcHeight_1);

			//Need to get osRect for this we need the NvDisplayHandle to get access to the displayName to be pased to win32
			NvDisplayHandle disp = NULL;
			NvAPI_DISP_GetDisplayHandleFromDisplayId(dispIds[dispIndex].displayId, &disp);
			//TODO errhandle
			NvAPI_ShortString displayName;
			error = NvAPI_GetAssociatedNvidiaDisplayName(disp, displayName);
			//TODO errhandle

			DEVMODEA dm = { 0 };
			dm.dmSize = sizeof(DEVMODEA);
			if (!EnumDisplaySettingsA(displayName, ENUM_CURRENT_SETTINGS, &dm)) {
				//TODO handle error
			}

			osRect.sX = dm.dmPosition.x;
			osRect.sY = dm.dmPosition.y;
			osRect.sWidth = dm.dmPelsWidth;
			osRect.sHeight = dm.dmPelsHeight;
			//scanoutRect.sHeight = 0;
			//viewportRect.sHeight = 0;

			printf(" desktopRect: sX = %6d, sY = %6d, sWidth = %6d sHeight = %6d\n", desktopRect.sX, desktopRect.sY, desktopRect.sWidth, desktopRect.sHeight);
			printf(" scanoutRect: sX = %6d, sY = %6d, sWidth = %6d sHeight = %6d\n", scanoutRect.sX, scanoutRect.sY, scanoutRect.sWidth, scanoutRect.sHeight);
			printf(" viewportRect: sX = %6d, sY = %6d, sWidth = %6d sHeight = %6d\n", viewportRect.sX, viewportRect.sY, viewportRect.sWidth, viewportRect.sHeight);
			printf(" osRect: sX = %6d, sY = %6d, sWidth = %6d sHeight = %6d\n", osRect.sX, osRect.sY, osRect.sWidth, osRect.sHeight);


			// desktopRect should now contain the area of the desktop which is scanned
			// out to the display given by the displayId.
			// scanoutRect should now contain the area of the scanout to which the 
			// desktopRect is scanned out. That will give information about the gpu 
			// internal output scaling before applying the warping and intensity control
			// However that doesn't give information about the scaling performed on 
			// the display side.

			// With this information the intensity and warping coordinates can be 
			// computed. The example here warps the desktopRect to a trapezoid similar
			// to the one seen in the diagram.	


			// warp vertices are defined in scanoutRect coordinates    					
			float	dstWidth = scanoutRect.sWidth / 2.0f;
			float	dstHeight = scanoutRect.sHeight;
			float	dstXShift = dstWidth / 2.0f;
			float	dstYShift = dstHeight / 2.0f;
			float	dstLeft = (float)scanoutRect.sX + dstXShift;
			float	dstTop = (float)scanoutRect.sY; //TODO play

			// Triangle strip with 4 vertices
			// The vertices are given as a 2D vertex strip because the warp
			// is a 2d operation. To be able to emulate 3d perspective correction, 
			// the texture coordinate contains 4 components, which needs to be
			// adjusted to get this correction.
			//
			// A trapezoid needs 4 vertices.
			// Format is xy for the vertex + yurq for the texture coordinate.
			// So we need 24 floats for that.
			//float vertices [4*6];
			//
			//  (0)  ----------------  (2)
			//       |             / |
			//       |            /  |
			//       |           /   |
			//       |          /    |
			//       |         /     |
			//       |        /      |
			//       |       /       |
			//       |      /        | 
			//       |     /         |
			//       |    /          |
			//       |   /           |
			//       |  /            |
			//       | /             |  
			//   (1) |---------------- (3)
			//


			// texture coordinates
			// warp texture coordinates are defined in desktopRect coordinates
			float srcLeft = (float)desktopRect.sX;
			float srcTop = (float)desktopRect.sY;
			float srcWidth = desktopRect.sWidth;
			float srcHeight = desktopRect.sHeight;
			srcHeight_1 = 1080;
			float var = 0.1f;


			float vertices[] =
				//     x                        y                       u                           v                   r       q
			{

				dstLeft,				         dstTop,			    srcLeft,					srcTop,				0.0f,	 1.0f,     // 0
				dstLeft - dstXShift,             dstTop + dstHeight,	srcLeft,					srcTop + srcHeight,	0.0f,	 0.5f,     // 1
				dstLeft + dstWidth + dstXShift,	 dstTop,			    srcLeft + srcWidth,			srcTop,				0.0f,	 1.0f,      // 2
				dstLeft + dstWidth,	             dstTop + dstHeight,	srcLeft + srcWidth,			srcTop + srcHeight,	0.0f,	 1.5f,     // 3
			};
			int maxnumvert = 4;
			//TL,BL,TR,BR
			float vertices_1_og[] =
				//     x                        y                  u                           v                   r       q
			{
				
				7* 4.010781,	            0* 4.010781,			(srcLeft_1)*1.0,					    (srcTop_1)*1.0,			                   0.0f,	 1.0f*0.249328f,     // 0
				5* 4.010781,                1078* 4.010781,	        (srcLeft_1)*1.0,					    (srcTop_1 + srcHeight_1)*1.0,	           0.0f,	 1.0f*0.249328f,     // 1
				1855* 4.010781,	            96* 4.010781,			(srcLeft_1 + srcWidth_1)*1.0,			(srcTop_1)*1.0,				               0.0f,	 1.0f*0.249328f,      // 2
				1916* 4.010781,	            1016* 4.010781,	        (srcLeft_1 + srcWidth_1)*1.0,			(srcTop_1 + srcHeight_1)*1.0,	           0.0f,	 1.0f*0.249328f     // 3
			};
			float vertices_1_og_2[] =
				//     x                        y                  u                           v                   r       q
			{

				7 * 5,	              0 * 5,			(srcLeft_1)*1.0,					    (srcTop_1)*1.0,			                   0.0f,	 1.0f*0.2f,     // 0
				5 * 5,                1078 *5,	        (srcLeft_1)*1.0,					    (srcTop_1 + srcHeight_1)*1.0,	           0.0f,	 1.0f*0.2f,     // 1
				1855 * 5,	          96 * 5,			(srcLeft_1 + srcWidth_1)*1.0,			(srcTop_1)*1.0,				               0.0f,	 1.0f*0.2f,      // 2
				1916 * 5,	          1016 * 5,	        (srcLeft_1 + srcWidth_1)*1.0,			(srcTop_1 + srcHeight_1)*1.0,	           0.0f,	 1.0f*0.2f     // 3
			};


			float vertices_1[] =
				//     x                        y                  u                           v                   r       q
			{

				1,	              1,			    srcLeft_1,					srcTop_1,				0.0f,	 1.0*1.0f,     // 0
				37,               1079,	            srcLeft_1,					srcTop_1 + srcHeight_1,	0.0f,	 1.0*1.0f,     // 1
				1914,	          27,			    srcLeft_1 + srcWidth_1,		srcTop_1,				0.0f,	 1.0*1.0f,      // 2
				1918,	          1057,	            srcLeft_1 + srcWidth_1,		srcTop_1 + srcHeight_1,	0.0f,	 1.0*1.0f,     // 3
			};

			float vertices_2[] =
				//     x                        y                  u                           v                   r       q
			{

				0,	                25,			        srcLeft_1,					srcTop_1,				            0.0f,	 1.0*1.0f,     // 0
				0,                  1066,	            srcLeft_1,					srcTop_1 + srcHeight_1,	            0.0f,	 1.0*1.0f,     // 1
				1919,	            51,			        srcLeft_1 + srcWidth_1,		srcTop_1,				            0.0f,	 1.0*1.0f,      // 4
				1893,	            1030,	            srcLeft_1 + srcWidth_1,		srcTop_1 + srcHeight_1,	            0.0f,	 1.0*1.0f,     // 3
			};

			float vertices_1_UV[] =
				//     x                        y                  u                           v                   r       q
			{

				480,	          0,			    1920,					0,				0.0f,	 1.0*1.0f,     // 0
				0,                900,	            1920,					1080,	        0.0f,	 1.0*1.0f,     // 1
				1920,	          0,			    3840,		            0,				0.0f,	 1.0*1.0f,      // 2
				1440,	          900,	            3840,		            1080,	        0.0f,	 1.0*1.0f,     // 3
			};

			float vertices_2_UV[] =
				//     x                        y                  u                           v                   r       q
			{
				 
				2.5 *23,	                1.0* 5,			        (srcLeft_1)*1.0,				    (srcTop_1)*1.0,				                0.0f,	 1.0*0.4f,     // 0
				2.5*0,                      1.0 *986,	            (srcLeft_1)*1.0,					(srcTop_1 + srcHeight_1)*1.0,	            0.0f,	 1.0*0.4f,     // 1
				2.5 *1894,	                1.0 *50,			    (srcLeft_1 + srcWidth_1)*1.0,		(srcTop_1)*1.0,				                0.0f,	 1.0*0.4f,      // 4
				2.5 *1917,	                1.0 *910,	            (srcLeft_1 + srcWidth_1)*1.0,		(srcTop_1 + srcHeight_1)*1.0,	            0.0f,	 1.0*0.4f
			};


			float vertices_wrong[] =
				//     x                        y                  u                           v                   r       q
			{

				344,	           1,			        srcLeft_1,					srcTop_1,				            0.0f,	 1.0f,     // 0
				1045,              77,	                srcLeft_1,					srcTop_1 + srcHeight_1,	            0.0f,	 1.0f,     // 1
				402,	           1919,			    srcLeft_1 + srcWidth_1,		srcTop_1,				            0.0f,	 1.0f,      // 4
				1044,	           1919,	            srcLeft_1 + srcWidth_1,		srcTop_1 + srcHeight_1,	            0.0f,	 1.0f,     // 3
			};

			float vertices_3[] =
				//     x                        y                  u                           v                   r       q
			{

				0,	           0,			        srcLeft_1,					srcTop_1,				0.0f,	 1.0f,     // 0
				0,             1200,	            srcLeft_1,					srcTop_1 + srcHeight_1,	0.0f,	 1.0f,     // 1
				1919,	       0,			        srcLeft_1 + srcWidth_1,		srcTop_1,				0.0f,	 1.0f,      // 2
				1919,	       1200,	            srcLeft_1 + srcWidth_1,		srcTop_1 + srcHeight_1,	0.0f,	 1.0f,     // 3
			};

			float vertices_4[] =
				//     x                        y                  u                           v                   r       q
			{

				0,	                    0,			        srcLeft_1,					srcTop_1,				            0.0f,	 1.0f,     // 0
				0,                      1200,	            srcLeft_1,					srcTop_1 + srcHeight_1,	            0.0f,	 1.0f,     // 1
				1919,	                0,			        srcLeft_1 + srcWidth_1,		srcTop_1,				            0.0f,	 1.0f,      // 4
				1919,	                1200,	            srcLeft_1 + srcWidth_1,		srcTop_1 + srcHeight_1,	            0.0f,	 1.0f,     // 3
			};



			printf("vertices2: %6.0f, %6.0f, %6.0f, %6.0f, %6.0f, %6.0f\n", vertices_2[0], vertices_2[1], vertices_2[2], vertices_2[3], vertices_2[4], vertices_2[5]);
			printf("vertices2: %6.0f, %6.0f, %6.0f, %6.0f, %6.0f, %6.0f\n", vertices_2[6], vertices_2[7], vertices_2[8], vertices_2[9], vertices_2[10], vertices_2[11]);
			printf("vertices2: %6.0f, %6.0f, %6.0f, %6.0f, %6.0f, %6.0f\n", vertices_2[12], vertices_2[13], vertices_2[14], vertices_2[15], vertices_2[16], vertices_2[17]);
			printf("vertices2: %6.0f, %6.0f, %6.0f, %6.0f, %6.0f, %6.0f\n", vertices_2[18], vertices_2[19], vertices_2[20], vertices_2[21], vertices_2[22], vertices_2[23]);
			printf("\n");
			printf("vertices1: %6.0f, %6.0f, %6.0f, %6.0f, %6.0f, %6.0f\n", vertices_1[0], vertices_1[1], vertices_1[2], vertices_1[3], vertices_1[4], vertices_1[5]);
			printf("vertices1: %6.0f, %6.0f, %6.0f, %6.0f, %6.0f, %6.0f\n", vertices_1[6], vertices_1[7], vertices_1[8], vertices_1[9], vertices_1[10], vertices_1[11]);
			printf("vertices1: %6.0f, %6.0f, %6.0f, %6.0f, %6.0f, %6.0f\n", vertices_1[12], vertices_1[13], vertices_1[14], vertices_1[15], vertices_1[16], vertices_1[17]);
			printf("vertices1: %6.0f, %6.0f, %6.0f, %6.0f, %6.0f, %6.0f\n", vertices_1[18], vertices_1[19], vertices_1[20], vertices_1[21], vertices_1[22], vertices_1[23]);
			//	printf("vertices: %6.0f, %6.0f, %6.0f, %6.0f, %6.0f, %6.0f\n",vertices[24],vertices[25],vertices[26],vertices[27],vertices[28],vertices[29]);

			// Demo Warping
			printf("Demo Warping\n");
			warpingData.version = NV_SCANOUT_WARPING_VER;
			warpingData.numVertices = maxnumvert;
			warpingData.vertexFormat = NV_GPU_WARPING_VERTICE_FORMAT_TRIANGLESTRIP_XYUVRQ;
			warpingData.textureRect = &osRect; 

			if (dispIds[dispIndex].displayId == 0x80061081)
			{
				warpingData.vertices = vertices_1;
				//warpingData.vertices = NULL;
				
			}
			else if (dispIds[dispIndex].displayId == 0x80061083) ////////////////screen1
			{
				warpingData.vertices = vertices_2;
			}
			else
			{
				warpingData.vertices = NULL;
			}

			//warpingData.vertices = vertices;

			// This call does the Warp
			error = NvAPI_GPU_SetScanoutWarping(dispIds[dispIndex].displayId, &warpingData, &maxNumVertices, &sticky);
			if (error != NVAPI_OK)
			{
				NvAPI_GetErrorMessage(error, estring);
				printf("NvAPI_GPU_SetScanoutWarping: %s\n", estring);
			}
			printf("afterwarping all the RECTS\n");
			printf(" desktopRect: sX = %6d, sY = %6d, sWidth = %6d sHeight = %6d\n", desktopRect.sX, desktopRect.sY, desktopRect.sWidth, desktopRect.sHeight);
			printf(" scanoutRect: sX = %6d, sY = %6d, sWidth = %6d sHeight = %6d\n", scanoutRect.sX, scanoutRect.sY, scanoutRect.sWidth, scanoutRect.sHeight);
			printf(" viewportRect: sX = %6d, sY = %6d, sWidth = %6d sHeight = %6d\n", viewportRect.sX, viewportRect.sY, viewportRect.sWidth, viewportRect.sHeight);
			printf(" osRect: sX = %6d, sY = %6d, sWidth = %6d sHeight = %6d\n", osRect.sX, osRect.sY, osRect.sWidth, osRect.sHeight);
			system("pause");

			//warpingData.vertices = NULL;
			//warpingData.numVertices = 0;

			//error = NvAPI_GPU_SetScanoutWarping(dispIds[dispIndex].displayId, &warpingData, &maxNumVertices, &sticky);

			//if (error != NVAPI_OK)  
			//{
			//	NvAPI_GetErrorMessage(error, estring);
			//	printf("NvAPI_GPU_SetScanoutWarping: %s\n", estring);
			//} 

			//// Demo Warp and Blend Antialiasing
			//printf("Demo Warping and Blend Antialiasing\n");

			//warpingData.version =  NV_SCANOUT_WARPING_VER; 
			//warpingData.numVertices = maxnumvert;
			//warpingData.vertexFormat = NV_GPU_WARPING_VERTICE_FORMAT_TRIANGLESTRIP_XYUVRQ;
			//warpingData.textureRect = &osRect; 


			//// This call does the Warp

			//error = NvAPI_GPU_SetScanoutWarping(dispIds[dispIndex].displayId, &warpingData, &maxNumVertices, &sticky);
			//if (error != NVAPI_OK)  
			//{ 
			//	NvAPI_GetErrorMessage(error, estring);
			//	printf("NvAPI_GPU_SetScanoutWarping: %s\n", estring);
			//}

   //         // This call changes the warp filter function to bicubic triangular
   //         printf("Changing warp resample method to  bicubic triangular\n");
   //         float unusedContainer;
   //         NV_GPU_SCANOUT_COMPOSITION_PARAMETER_VALUE parameterValue = NV_GPU_SCANOUT_COMPOSITION_PARAMETER_VALUE_WARPING_RESAMPLING_METHOD_BICUBIC_TRIANGULAR;
   //         error = NvAPI_GPU_SetScanoutCompositionParameter(dispIds[dispIndex].displayId, NV_GPU_SCANOUT_COMPOSITION_PARAMETER_WARPING_RESAMPLING_METHOD, parameterValue, &unusedContainer);

   //                  if (error != NVAPI_OK)  
   //                  { 
   //                        NvAPI_GetErrorMessage(error, estring);
   //                        printf("NvAPI_GPU_SetScanoutCompositionParameter: %s\n", estring);
   //                  }

			//system("pause");

   //         // After a pause, set back to unwarped and un-antialiased
   //         parameterValue = NV_GPU_SCANOUT_COMPOSITION_PARAMETER_VALUE_WARPING_RESAMPLING_METHOD_BILINEAR;
   //         error = NvAPI_GPU_SetScanoutCompositionParameter(dispIds[dispIndex].displayId, NV_GPU_SCANOUT_COMPOSITION_PARAMETER_WARPING_RESAMPLING_METHOD, parameterValue, &unusedContainer);

   //                  if (error != NVAPI_OK)  
   //                  { 
   //                        NvAPI_GetErrorMessage(error, estring);
   //                        printf("NvAPI_GPU_SetScanoutCompositionParameter: %s\n", estring);
   //                  }

			//warpingData.vertices = NULL;
			//warpingData.numVertices = 0;

			//
			//error = NvAPI_GPU_SetScanoutWarping(dispIds[dispIndex].displayId, &warpingData, &maxNumVertices, &sticky);

			//if (error != NVAPI_OK)  
			//{
			//	NvAPI_GetErrorMessage(error, estring);
			//	printf("NvAPI_GPU_SetScanoutWarping: %s\n", estring);
			//} 

			//// Demonstration of Intensity adjustment
			//printf("Demo Intensity Adjustment\n");

			//NV_SCANOUT_INTENSITY_DATA intensityData; 
			//// As per pixel intensity control example we specify only one intensity 
			//// value which is interpolated over the scanout area and affecting 
			//// all pixels by that. It's also possible to specify much more intensity
			//// values than scanout pixels.

			//float intensityTexture[12]        = { 0.95f, 0.95f, 0.95f, 0.75f, 0.75f, 0.75f, 0.25f, 0.25f, 0.25f, 0.05f, 0.05f, 0.05f} ;


			//float offsetTexture[2]        = { 0.0f, 0.1f} ;
			//intensityData.version           = NV_SCANOUT_INTENSITY_DATA_VER;
			//intensityData.width             = 4;
			//intensityData.height            = 1;
			//intensityData.blendingTexture   = intensityTexture;
			//intensityData.offsetTexture		= offsetTexture;
			//intensityData.offsetTexChannels = 1;kl

			//int sticky = 0;

			//// This call does the intensity map
			//error =  NvAPI_GPU_SetScanoutIntensity(dispIds[dispIndex].displayId, &intensityData, &sticky);

			//if (error != NVAPI_OK)  
			//{
			//	NvAPI_GetErrorMessage(error, estring);
			//	printf("NvAPI_GPU_SetScanoutIntensity: %s\n", estring);
			//} 

			//system("pause");

			//// After a pause, set back to normal intensity

			//intensityData.blendingTexture = NULL;

			//error =  NvAPI_GPU_SetScanoutIntensity(dispIds[dispIndex].displayId, &intensityData, &sticky);
			//if (error != NVAPI_OK)  
			//{
			//	NvAPI_GetErrorMessage(error, estring);
			//	printf("NvAPI_GPU_SetScanoutIntensity: %s\n", estring);
			//} 
		} //end of for displays
		warpingData.vertices = NULL;

		error = NvAPI_GPU_SetScanoutWarping(0x80061081, &warpingData, &maxNumVertices, &sticky);
		if (error != NVAPI_OK)
		{
			NvAPI_GetErrorMessage(error, estring);
			printf("NvAPI_GPU_SetScanoutWarping: %s\n", estring);
		}
		system("pause");
		error = NvAPI_GPU_SetScanoutWarping(0x80061083, &warpingData, &maxNumVertices, &sticky);
		if (error != NVAPI_OK)
		{
			NvAPI_GetErrorMessage(error, estring);
			printf("NvAPI_GPU_SetScanoutWarping: %s\n", estring);
		}
		system("pause");
		delete[] dispIds;
	}	//end of loop gpus

}




//
//#include "OculusSDK/LibOVR/Include/OVRVersion.h"
//#include "OculusSDK/LibOVR/Include/OVR.h"
////#include "OculusSDK/LibOVR/Src/OVRCAPI.h"
//
//using namespace OVR;
//
//
////OCULUS TESTING
//ovrHmd hmd;
//ovrHmdDesc hmdDesc;
//ovrFrameTiming      hmdFrameTiming;
//SensorState			ss;
//Transformf          HeadPose;
//
//
//
//void InitOculus();
//void CompileOculusShaders();
//bool oculusEnabled = true;
//
////const char* oFrag = "uniform sampler2D baseMap; varying vec2 texCoord;  void main (void) { gl_FragColor =texture2D(baseMap, texCoord); }";
//const char* oVert="varying vec2 texCoord; void main(void) {gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; texCoord = vec2(gl_MultiTexCoord0);}";
//
//const char* oFragR = "//  Author:    Marc Wacker, Roman Kühne\n"
//"//  Date:      November 2013\n"
//"//             This software is provide under the GNU General Public License\n"
//"//             Please visit: http://opensource.org/licenses/GPL-3.0\n"
//"//#############################################################################\n"
//"uniform sampler2D	baseMap;\n"
//"uniform float		lensCenterOffset=0.151976;\n"
//"uniform vec4		hmdWarpParam=vec4(1,0.22f,0.24f,0.0f); \n"
//"uniform vec2		scale=vec2(1,1);\n"
//"uniform vec2		scaleIn=vec2(1,1);\n"
//"\n"
//"varying vec2		texCoord;\n"
//"\n"
//"vec2 lensCenter = vec2(0.5, 0.5);\n"
//"vec2 ScreenCenter = vec2(0.5, 0.5);\n"
//"\n"
//"// Scales input texture coordinates for distortion.\n"
//"vec2 HmdWarp(vec2 in01)\n"
//"{\n"
//"	vec2 theta=(in01-lensCenter)*scaleIn*0.8;"
//"	float rSq = theta.x * theta.x *4 + theta.y * theta.y*4;\n"
//"	vec2 rvector = theta * (hmdWarpParam.x + hmdWarpParam.y * rSq +\n"
//"		hmdWarpParam.z * rSq * rSq +\n"
//"		hmdWarpParam.w * rSq * rSq * rSq);\n"
//"	return lensCenter + scale * rvector;\n"
//"}\n"
//"\n"
//"void main()\n"
//"{\n"
//"	lensCenter.x=lensCenter.x-lensCenterOffset;"
//"	vec2 tc = HmdWarp(texCoord.xy);\n"
//
//
//"	if (any(bvec2(clamp(tc,ScreenCenter-vec2(0.5,0.5), ScreenCenter+vec2(0.5,0.5)) - tc)))\n"
//"	{\n"
//"		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
//"		return;\n"
//"	}\n"
//
//"\n"
//"	gl_FragColor = texture2D(baseMap, tc);\n"
//"}\n";
//
//const char* oFragL = "//  Author:    Marc Wacker, Roman Kühne\n"
//"//  Date:      November 2013\n"
//"//             This software is provide under the GNU General Public License\n"
//"//             Please visit: http://opensource.org/licenses/GPL-3.0\n"
//"//#############################################################################\n"
//"uniform sampler2D	baseMap;\n"
//"uniform float		lensCenterOffset=0.151976;\n"
//"uniform vec4		hmdWarpParam=vec4(1,0.22f,0.24f,0.0f); \n"
//"uniform vec2		scale=vec2(1,1);\n"
//"uniform vec2		scaleIn=vec2(1,1);\n"
//"\n"
//"varying vec2		texCoord;\n"
//"\n"
//"vec2 lensCenter = vec2(0.5, 0.5);\n"
//"vec2 ScreenCenter = vec2(0.5, 0.5);\n"
//"\n"
//"// Scales input texture coordinates for distortion.\n"
//"vec2 HmdWarp(vec2 in01)\n"
//"{\n"
//"	vec2 theta=(in01-lensCenter)*scaleIn*0.8;"
//"	float rSq = theta.x * theta.x *4 + theta.y * theta.y*4;\n"
//"	vec2 rvector = theta * (hmdWarpParam.x + hmdWarpParam.y * rSq +\n"
//"		hmdWarpParam.z * rSq * rSq +\n"
//"		hmdWarpParam.w * rSq * rSq * rSq);\n"
//"	return lensCenter + scale * rvector;\n"
//"}\n"
//"\n"
//"void main()\n"
//"{\n"
//"	lensCenter.x = lensCenter.x + lensCenterOffset; "
//"	vec2 tc = HmdWarp(texCoord.xy);\n"
//
//"	if (any(bvec2(clamp(tc,ScreenCenter-vec2(0.5,0.5), ScreenCenter+vec2(0.5,0.5)) - tc)))\n"
//"	{\n"
//"		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
//"		return;\n"
//"	}\n"
//
//"\n"
//"	gl_FragColor = texture2D(baseMap, tc);\n"
//"}\n";
//
//void CalculateHmdValues()
//{
//	// Initialize eye rendering information for ovrHmd_Configure.
//	// The viewport sizes are re-computed in case RenderTargetSize changed due to HW limitations.
//	ovrFovPort eyeFov[2];
//	eyeFov[0] = hmdDesc.DefaultEyeFov[0];
//	eyeFov[1] = hmdDesc.DefaultEyeFov[1];
//
//	float FovSideTanLimit = FovPort::Max(hmdDesc.MaxEyeFov[0], hmdDesc.MaxEyeFov[1]).GetMaxSideTan();
//	float FovSideTanMax = FovPort::Max(hmdDesc.DefaultEyeFov[0], hmdDesc.DefaultEyeFov[1]).GetMaxSideTan();
//
//	// Clamp Fov based on our dynamically adjustable FovSideTanMax.
//	// Most apps should use the default, but reducing Fov does reduce rendering cost.
//	eyeFov[0] = FovPort::Min(eyeFov[0], FovPort(FovSideTanMax));
//	eyeFov[1] = FovPort::Min(eyeFov[1], FovPort(FovSideTanMax));
//	/*
//	Sizei tex0Size = EnsureRendertargetAtLeastThisBig(Rendertarget_Left, recommenedTex0Size);
//	Sizei tex1Size = EnsureRendertargetAtLeastThisBig(Rendertarget_Right, recommenedTex1Size);
//
//	EyeRenderSize[0] = Sizei::Min(tex0Size, recommenedTex0Size);
//	EyeRenderSize[1] = Sizei::Min(tex1Size, recommenedTex1Size);
//
//	// Store texture pointers and viewports that will be passed for rendering.
//	EyeTexture[0] = RenderTargets[Rendertarget_Left].Tex;
//	EyeTexture[0].Header.TextureSize = tex0Size;
//	EyeTexture[0].Header.RenderViewport = Recti(EyeRenderSize[0]);
//	EyeTexture[1] = RenderTargets[Rendertarget_Right].Tex;
//	EyeTexture[1].Header.TextureSize = tex1Size;
//	EyeTexture[1].Header.RenderViewport = Recti(EyeRenderSize[1]);
//
//	// Hmd caps.
//	unsigned hmdCaps = 0;
//	if (false)//IsLowPersistence)
//		hmdCaps |= ovrHmdCap_LowPersistence;
//	if (false)//DynamicPrediction)
//		hmdCaps |= ovrHmdCap_DynamicPrediction;
//
//	ovrHmd_SetEnabledCaps(hmd, hmdCaps);
//
//
//	ovrRenderAPIConfig config = pRender->Get_ovrRenderAPIConfig();
//	unsigned           distortionCaps = ovrDistortionCap_Chromatic;
//	if (TimewarpEnabled)
//		distortionCaps |= ovrDistortionCap_TimeWarp;
//
//	if (!ovrHmd_ConfigureRendering(Hmd, &config, distortionCaps,
//		eyeFov, EyeRenderDesc))
//	{
//		// Fail exit? TBD
//		return;
//	}
//
//
//
//	// ovrHmdCap_LatencyTest - enables internal latency feedback
//	unsigned sensorCaps = ovrSensorCap_Orientation | ovrSensorCap_YawCorrection;
//	if (PositionTrackingEnabled)
//		sensorCaps |= ovrSensorCap_Position;
//
//	if (StartSensorCaps != sensorCaps)
//	{
//		ovrHmd_StartSensor(Hmd, sensorCaps, 0);
//		StartSensorCaps = sensorCaps;
//	}
//
//	// Calculate projections
//	Projection[0] = ovrMatrix4f_Projection(EyeRenderDesc[0].Fov, 0.01f, 10000.0f, true);
//	Projection[1] = ovrMatrix4f_Projection(EyeRenderDesc[1].Fov, 0.01f, 10000.0f, true);
//
//	float    orthoDistance = 0.8f; // 2D is 0.8 meter from camera
//	Vector2f orthoScale0 = Vector2f(1.0f) / Vector2f(EyeRenderDesc[0].PixelsPerTanAngleAtCenter);
//	Vector2f orthoScale1 = Vector2f(1.0f) / Vector2f(EyeRenderDesc[1].PixelsPerTanAngleAtCenter);
//	*/
//
//	/*
//	OrthoProjection[0] = ovrMatrix4f_OrthoSubProjection(Projection[0], orthoScale0, orthoDistance,
//		EyeRenderDesc[0].ViewAdjust.x);
//	OrthoProjection[1] = ovrMatrix4f_OrthoSubProjection(Projection[1], orthoScale1, orthoDistance,
//		EyeRenderDesc[1].ViewAdjust.x);*/
//}
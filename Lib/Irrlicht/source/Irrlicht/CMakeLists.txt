set(PROJECT_NAME Irrlicht)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "../../tools/ResFile/src/resFile.cpp"
    "encoding.cpp"
    "StdAfx.cpp"
)
source_group("" FILES ${no_group_source_files})

set(doc
    "../../readme.txt"
)
source_group("doc" FILES ${doc})

set(gui_impl
    "../../include/BuiltInFont.h"
    "../../include/CDefaultGUIElementFactory.h"
    "../../include/CGUIButton.h"
    "../../include/CGUICheckBox.h"
    "../../include/CGUIColorSelectDialog.h"
    "../../include/CGUIComboBox.h"
    "../../include/CGUIContextMenu.h"
    "../../include/CGUIEditBox.h"
    "../../include/CGUIEnvironment.h"
    "../../include/CGUIFileOpenDialog.h"
    "../../include/CGUIFont.h"
    "../../include/CGUIFontVector.h"
    "../../include/CGUIImage.h"
    "../../include/CGUIInOutFader.h"
    "../../include/CGUIListBox.h"
    "../../include/CGUIMenu.h"
    "../../include/CGUIMeshViewer.h"
    "../../include/CGUIMessageBox.h"
    "../../include/CGUIModalScreen.h"
    "../../include/CGUIScrollBar.h"
    "../../include/CGUISkin.h"
    "../../include/CGUISpinBox.h"
    "../../include/CGUISpriteBank.h"
    "../../include/CGUIStaticText.h"
    "../../include/CGUITabControl.h"
    "../../include/CGUITable.h"
    "../../include/CGUIToolBar.h"
    "../../include/CGUITTFont.h"
    "../../include/CGUIWindow.h"
    "CDefaultGUIElementFactory.cpp"
    "CGUIButton.cpp"
    "CGUICheckBox.cpp"
    "CGUIColorSelectDialog.cpp"
    "CGUIComboBox.cpp"
    "CGUIContextMenu.cpp"
    "CGUIEditBox.cpp"
    "CGUIEnvironment.cpp"
    "CGUIFileOpenDialog.cpp"
    "CGUIFont.cpp"
    "CGUIFontVector.cpp"
    "CGUIImage.cpp"
    "CGUIInOutFader.cpp"
    "CGUIListBox.cpp"
    "CGUIMenu.cpp"
    "CGUIMeshViewer.cpp"
    "CGUIMessageBox.cpp"
    "CGUIModalScreen.cpp"
    "CGUIScrollBar.cpp"
    "CGUISkin.cpp"
    "CGUISpinBox.cpp"
    "CGUISpriteBank.cpp"
    "CGUIStaticText.cpp"
    "CGUITabControl.cpp"
    "CGUITable.cpp"
    "CGUIToolBar.cpp"
    "CGUITTFont.cpp"
    "CGUIWindow.cpp"
)
source_group("gui impl" FILES ${gui_impl})

set(include
    "../../include/BeginPack.h"
    "../../include/CTextureManager.h"
    "../../include/EndPack.h"
    "../../include/IEventReceiver.h"
    "../../include/ILogger.h"
    "../../include/IOSOperator.h"
    "../../include/IReferenceCounted.h"
    "../../include/IrrCompileConfig.h"
    "../../include/IrrCompileDefaultConfig.h"
    "../../include/IrrCompileGameConfig.h"
    "../../include/irrlicht.h"
    "../../include/IrrlichtDevice.h"
    "../../include/irrProcessBufferHeap.h"
    "../../include/irrTypes.h"
    "../../include/ITimer.h"
    "../../include/Keycodes.h"
    "../../include/SIrrCreationParameters.h"
    "../../include/SKeyMap.h"
)
source_group("include" FILES ${include})

set(include__core
    "../../include/aabbox3d.h"
    "../../include/dimension2d.h"
    "../../include/heapsort.h"
    "../../include/irrAllocator.h"
    "../../include/irrArray.h"
    "../../include/irrList.h"
    "../../include/irrMap.h"
    "../../include/irrMath.h"
    "../../include/irrString.h"
    "../../include/line2d.h"
    "../../include/line3d.h"
    "../../include/matrix4.h"
    "../../include/plane3d.h"
    "../../include/position2d.h"
    "../../include/quaternion.h"
    "../../include/rect.h"
    "../../include/triangle3d.h"
    "../../include/UTF8.h"
    "../../include/vector2d.h"
    "../../include/vector3d.h"
)
source_group("include\\core" FILES ${include__core})

set(include__gui
    "../../include/EGUIElementTypes.h"
    "../../include/EMessageBoxFlags.h"
    "../../include/ICursorControl.h"
    "../../include/IGUIButton.h"
    "../../include/IGUICheckBox.h"
    "../../include/IGUIComboBox.h"
    "../../include/IGUIContextMenu.h"
    "../../include/IGUIEditBox.h"
    "../../include/IGUIElement.h"
    "../../include/IGUIElementFactory.h"
    "../../include/IGUIEnvironment.h"
    "../../include/IGUIFileOpenDialog.h"
    "../../include/IGUIFont.h"
    "../../include/IGUIFontVector.h"
    "../../include/IGUIImage.h"
    "../../include/IGUIInOutFader.h"
    "../../include/IGUIListBox.h"
    "../../include/IGUIMeshViewer.h"
    "../../include/IGUIScrollBar.h"
    "../../include/IGUISkin.h"
    "../../include/IGUISpinBox.h"
    "../../include/IGUISpriteBank.h"
    "../../include/IGUIStaticText.h"
    "../../include/IGUITabControl.h"
    "../../include/IGUIToolbar.h"
    "../../include/IGUITTFont.h"
    "../../include/IGUIWindow.h"
)
source_group("include\\gui" FILES ${include__gui})

set(include__io
    "../../include/IAttributeExchangingObject.h"
    "../../include/IAttributes.h"
    "../../include/IFileList.h"
    "../../include/IFileSystem.h"
    "../../include/IReadFile.h"
    "../../include/irrXML.h"
    "../../include/IWriteFile.h"
    "../../include/IXMLReader.h"
    "../../include/IXMLWriter.h"
)
source_group("include\\io" FILES ${include__io})

set(include__scene
    "../../include/ECullingTypes.h"
    "../../include/EDebugSceneTypes.h"
    "../../include/EMeshWriterEnums.h"
    "../../include/EPrimitiveType.h"
    "../../include/ESceneNodeAnimatorTypes.h"
    "../../include/ESceneNodeTypes.h"
    "../../include/IAnimatedMesh.h"
    "../../include/IAnimatedMeshMD2.h"
    "../../include/IAnimatedMeshMD3.h"
    "../../include/IAnimatedMeshSceneNode.h"
    "../../include/IAnimationTrack.h"
    "../../include/IAnimator.h"
    "../../include/IBillboardSceneNode.h"
    "../../include/ICameraSceneNode.h"
    "../../include/IChannelAnimator.h"
    "../../include/IDummyTransformationSceneNode.h"
    "../../include/ILightSceneNode.h"
    "../../include/IMesh.h"
    "../../include/IMeshBuffer.h"
    "../../include/IMeshCache.h"
    "../../include/IMeshLoader.h"
    "../../include/IMeshManipulator.h"
    "../../include/IMeshSceneNode.h"
    "../../include/IMeshWriter.h"
    "../../include/IMetaTriangleSelector.h"
    "../../include/IMorphingMesh.h"
    "../../include/IParticleAffector.h"
    "../../include/IParticleEmitter.h"
    "../../include/IParticleSystemSceneNode.h"
    "../../include/IQ3LevelMesh.h"
    "../../include/ISceneCollisionManager.h"
    "../../include/ISceneManager.h"
    "../../include/ISceneNode.h"
    "../../include/ISceneNodeAnimator.h"
    "../../include/ISceneNodeAnimatorChannelLibrary.h"
    "../../include/ISceneNodeAnimatorCollisionResponse.h"
    "../../include/ISceneNodeAnimatorFactory.h"
    "../../include/ISceneNodeFactory.h"
    "../../include/IShadowVolumeSceneNode.h"
    "../../include/ISkinnedMesh.h"
    "../../include/ITerrainSceneNode.h"
    "../../include/ITextSceneNode.h"
    "../../include/ITimeController.h"
    "../../include/ITriangleSelector.h"
    "../../include/SAnimatedMesh.h"
    "../../include/SceneParameters.h"
    "../../include/SMesh.h"
    "../../include/SMeshBuffer.h"
    "../../include/SMeshBufferLightMap.h"
    "../../include/SMeshBufferTangents.h"
    "../../include/SParticle.h"
    "../../include/SViewFrustum.h"
)
source_group("include\\scene" FILES ${include__scene})

set(include__video
    "../../include/EDriverAllocationResult.h"
    "../../include/EDriverTypes.h"
    "../../include/EVertexAttributes.h"
    "../../include/EVertexType.h"
    "../../include/IBatchBinding.h"
    "../../include/IDriverBinding.h"
    "../../include/IGPUProgrammingServices.h"
    "../../include/IImage.h"
    "../../include/IImageLoader.h"
    "../../include/IMaterialRenderer.h"
    "../../include/IMaterialRendererServices.h"
    "../../include/irrTexGen.h"
    "../../include/IShaderConstantSetCallBack.h"
    "../../include/ITexture.h"
    "../../include/IVideoDriver.h"
    "../../include/IVideoModeList.h"
    "../../include/S3DVertex.h"
    "../../include/S3DVertexComponentArrays.h"
    "../../include/SColor.h"
    "../../include/SExposedVideoData.h"
    "../../include/SLight.h"
    "../../include/SMaterial.h"
    "S3DVertexComponentArrays.cpp"
)
source_group("include\\video" FILES ${include__video})

set(io_impl
    "CAttributes.cpp"
    "../../include/CAttributes.h"
    "CFileList.cpp"
    "../../include/CFileList.h"
    "CFileSystem.cpp"
    "../../include/CFileSystem.h"
    "CLimitReadFile.cpp"
    "../../include/CLimitReadFile.h"
    "CMemoryReadFile.cpp"
    "../../include/CMemoryReadFile.h"
    "CPakReader.cpp"
    "../../include/CPakReader.h"
    "CReadFile.cpp"
    "../../include/CReadFile.h"
    "CWriteFile.cpp"
    "../../include/CWriteFile.h"
    "CXMLReader.cpp"
    "../../include/CXMLReader.h"
    "../../include/CXMLReaderImpl.h"
    "CXMLWriter.cpp"
    "../../include/CXMLWriter.h"
    "CZipReader.cpp"
    "../../include/CZipReader.h"
    "irrXML.cpp"
)
source_group("io impl" FILES ${io_impl})

set(irr_impl
    "../../include/CIrrFactory.h"
    "../../include/irros.h"
    "CIrrDeviceLinux.cpp"
    "../../include/CIrrDeviceLinux.h"
    "CIrrDeviceSDL.cpp"
    "../../include/CIrrDeviceSDL.h"
    "CIrrDeviceStub.cpp"
    "../../include/CIrrDeviceStub.h"
    "CIrrDeviceWii.cpp"
    "../../include/CIrrDeviceWii.h"
    "CIrrDeviceWin32.cpp"
    "../../include/CIrrDeviceWin32.h"
    "CIrrDeviceWinCE.cpp"
    "../../include/CIrrDeviceWinCE.h"
    "CIrrFactory.cpp"
    "CLogger.cpp"
    "../../include/CLogger.h"
    "COSOperator.cpp"
    "../../include/COSOperator.h"
    "../../include/CTimer.h"
    "Irrlicht.cpp"
    "IrrlichtDevice.cpp"
    "irros.cpp"
    "irrProcessBufferHeap.cpp"
)
source_group("irr impl" FILES ${irr_impl})

set(irr_impl__extern__jpeglib
    "jpeglib/cderror.h"
    "jpeglib/cdjpeg.c"
    "jpeglib/cdjpeg.h"
    "jpeglib/jcapimin.c"
    "jpeglib/jcapistd.c"
    "jpeglib/jccoefct.c"
    "jpeglib/jccolor.c"
    "jpeglib/jcdctmgr.c"
    "jpeglib/jchuff.c"
    "jpeglib/jchuff.h"
    "jpeglib/jcinit.c"
    "jpeglib/jcmainct.c"
    "jpeglib/jcmarker.c"
    "jpeglib/jcmaster.c"
    "jpeglib/jcomapi.c"
    "jpeglib/jconfig.h"
    "jpeglib/jcparam.c"
    "jpeglib/jcphuff.c"
    "jpeglib/jcprepct.c"
    "jpeglib/jcsample.c"
    "jpeglib/jctrans.c"
    "jpeglib/jdapimin.c"
    "jpeglib/jdapistd.c"
    "jpeglib/jdatadst.c"
    "jpeglib/jdatasrc.c"
    "jpeglib/jdcoefct.c"
    "jpeglib/jdcolor.c"
    "jpeglib/jdct.h"
    "jpeglib/jddctmgr.c"
    "jpeglib/jdhuff.c"
    "jpeglib/jdhuff.h"
    "jpeglib/jdinput.c"
    "jpeglib/jdmainct.c"
    "jpeglib/jdmarker.c"
    "jpeglib/jdmaster.c"
    "jpeglib/jdmerge.c"
    "jpeglib/jdphuff.c"
    "jpeglib/jdpostct.c"
    "jpeglib/jdsample.c"
    "jpeglib/jdtrans.c"
    "jpeglib/jerror.c"
    "jpeglib/jerror.h"
    "jpeglib/jfdctflt.c"
    "jpeglib/jfdctfst.c"
    "jpeglib/jfdctint.c"
    "jpeglib/jidctflt.c"
    "jpeglib/jidctfst.c"
    "jpeglib/jidctint.c"
    "jpeglib/jidctred.c"
    "jpeglib/jinclude.h"
    "jpeglib/jmemirr.c"
    "jpeglib/jmemmgr.c"
    "jpeglib/jmemsys.h"
    "jpeglib/jmorecfg.h"
    "jpeglib/jpegint.h"
    "jpeglib/jpeglib.h"
    "jpeglib/jquant1.c"
    "jpeglib/jquant2.c"
    "jpeglib/jutils.c"
    "jpeglib/jversion.h"
)
source_group("irr impl\\extern\\jpeglib" FILES ${irr_impl__extern__jpeglib})

set(irr_impl__extern__libpng
    "libpng/png.c"
    "libpng/png.h"
    "libpng/pngconf.h"
    "libpng/pngerror.c"
    "libpng/pngget.c"
    "libpng/pngmem.c"
    "libpng/pngpread.c"
    "libpng/pngread.c"
    "libpng/pngrio.c"
    "libpng/pngrtran.c"
    "libpng/pngrutil.c"
    "libpng/pngset.c"
    "libpng/pngtrans.c"
    "libpng/pngwio.c"
    "libpng/pngwrite.c"
    "libpng/pngwtran.c"
    "libpng/pngwutil.c"
)
source_group("irr impl\\extern\\libpng" FILES ${irr_impl__extern__libpng})

set(irr_impl__extern__particlesystem
    "particlesystem/include/ParameterNames.h"
    "particlesystem/include/Particle.h"
    "particlesystem/include/ParticleContext.h"
    "particlesystem/include/ParticleSystem.h"
    "particlesystem/include/PBehaviorModel.h"
    "particlesystem/include/PDomain.h"
    "particlesystem/include/PForce.h"
    "particlesystem/include/PForceImpl.h"
    "particlesystem/include/PRenderData.h"
    "particlesystem/include/PSManager.h"
    "particlesystem/include/PSUtils.h"
    "particlesystem/scr/Particle.cpp"
    "particlesystem/scr/ParticleContext.cpp"
    "particlesystem/scr/ParticleSystem.cpp"
    "particlesystem/scr/PBehaviorModel.cpp"
    "particlesystem/scr/PDomain.cpp"
    "particlesystem/scr/PSManager.cpp"
)
source_group("irr impl\\extern\\particlesystem" FILES ${irr_impl__extern__particlesystem})

set(irr_impl__extern__particlesystem__behaviormodels
    "particlesystem/include/BehaviorModels/PColorModel.h"
    "particlesystem/include/BehaviorModels/PEmitterModel.h"
    "particlesystem/include/BehaviorModels/PForcesModel.h"
    "particlesystem/include/BehaviorModels/PGenerationModel.h"
    "particlesystem/include/BehaviorModels/PLifeModel.h"
    "particlesystem/include/BehaviorModels/PMotionModel.h"
    "particlesystem/include/BehaviorModels/PRenderDataModel.h"
    "particlesystem/include/BehaviorModels/PSizeModel.h"
    "particlesystem/include/BehaviorModels/PSpinModel.h"
)
source_group("irr impl\\extern\\particlesystem\\behaviormodels" FILES ${irr_impl__extern__particlesystem__behaviormodels})

set(irr_impl__extern__pvrt
    "PVRT/PVRTDecompress.cpp"
    "PVRT/PVRTDecompress.h"
)
source_group("irr impl\\extern\\pvrt" FILES ${irr_impl__extern__pvrt})

set(irr_impl__extern__zlib
    "zlib/adler32.c"
    "zlib/compress.c"
    "zlib/crc32.c"
    "zlib/crc32.h"
    "zlib/deflate.c"
    "zlib/deflate.h"
    "zlib/inffast.c"
    "zlib/inffast.h"
    "zlib/inffixed.h"
    "zlib/inflate.c"
    "zlib/inftrees.c"
    "zlib/inftrees.h"
    "zlib/trees.c"
    "zlib/trees.h"
    "zlib/uncompr.c"
    "zlib/zconf.h"
    "zlib/zlib.h"
    "zlib/zutil.c"
    "zlib/zutil.h"
)
source_group("irr impl\\extern\\zlib" FILES ${irr_impl__extern__zlib})

set(scene_impl
    "../../include/CSceneManager.h"
    "../../include/ECullingResult.h"
    "CDefaultSceneNodeAnimatorFactory.cpp"
    "../../include/CDefaultSceneNodeAnimatorFactory.h"
    "CDefaultSceneNodeFactory.cpp"
    "../../include/CDefaultSceneNodeFactory.h"
    "CGeometryCreator.cpp"
    "../../include/CGeometryCreator.h"
    "CMeshCache.cpp"
    "../../include/CMeshCache.h"
    "CMeshManipulator.cpp"
    "../../include/CMeshManipulator.h"
    "CSceneManager.cpp"
    "IMesh.cpp"
    "IMeshBuffer.cpp"
    "../../include/OctTree.h"
)
source_group("scene impl" FILES ${scene_impl})

set(scene_impl__animators
    "../../include/CAnimationClip.h"
    "CAnimationTrack.cpp"
    "../../include/CAnimationTrack.h"
    "../../include/CAnimationTrackLibrary.h"
    "CChannelAnimator.cpp"
    "../../include/CChannelAnimator.h"
    "CChannelBlender.cpp"
    "../../include/CChannelBlender.h"
    "CSceneNodeAnimatorChannelLibrary.cpp"
    "../../include/CSceneNodeAnimatorChannelLibrary.h"
    "CSceneNodeAnimatorCollisionResponse.cpp"
    "../../include/CSceneNodeAnimatorCollisionResponse.h"
    "CSceneNodeAnimatorDelete.cpp"
    "../../include/CSceneNodeAnimatorDelete.h"
    "CSceneNodeAnimatorFlyCircle.cpp"
    "../../include/CSceneNodeAnimatorFlyCircle.h"
    "CSceneNodeAnimatorFlyStraight.cpp"
    "../../include/CSceneNodeAnimatorFlyStraight.h"
    "CSceneNodeAnimatorFollowSpline.cpp"
    "../../include/CSceneNodeAnimatorFollowSpline.h"
    "CSceneNodeAnimatorRotation.cpp"
    "../../include/CSceneNodeAnimatorRotation.h"
    "CSceneNodeAnimatorTexture.cpp"
    "../../include/CSceneNodeAnimatorTexture.h"
    "../../include/CTimeController.h"
    "../../include/CTimelineController.h"
    "ISceneNodeAnimator.cpp"
)
source_group("scene impl\\animators" FILES ${scene_impl__animators})

set(scene_impl__batching
    "../../include/CBatchBuffer.h"
    "../../include/CBatchGridSceneNode.h"
    "../../include/CBatchGridSceneNode_impl.h"
    "../../include/CBatchMesh.h"
    "../../include/CBatchMesh_impl.h"
    "../../include/CBatchSceneNode.h"
    "../../include/CBatchSceneNode_impl.h"
    "../../include/IBatchList.h"
    "../../include/IBatchMesh.h"
    "../../include/IBatchSceneNode.h"
    "CBatchBuffer.cpp"
    "CBatchMesh.cpp"
    "IBatchList.cpp"
    "IBatchMesh.cpp"
    "IBatchSceneNode.cpp"
)
source_group("scene impl\\batching" FILES ${scene_impl__batching})

set(scene_impl__collada
    "../../include/CColladaAnimationTrack.h"
    "../../include/CColladaAnimationTrackFloat.h"
    "../../include/CColladaAnimationTrackImageFileList.h"
    "../../include/CColladaAnimationTrackLightColor.h"
    "../../include/CColladaAnimationTrackOffsetUV.h"
    "../../include/CColladaAnimationTrackPosition.h"
    "../../include/CColladaAnimationTrackQuaternion.h"
    "../../include/CColladaAnimationTrackTransparency.h"
    "../../include/CColladaAnimationTrackVector3d.h"
    "../../include/CColladaAnimationTrackVisibility.h"
    "../../include/CColladaAnimationTrackWeight.h"
    "../../include/CColladaBillboardSceneNode.h"
    "../../include/CColladaBinaryFileLoader.h"
    "../../include/CColladaCameraSceneNode.h"
    "../../include/CColladaConverter.h"
    "../../include/CColladaDatabase.h"
    "../../include/CColladaDynamicAnimationSet.h"
    "../../include/CColladaEventsManager.h"
    "../../include/CColladaFactory.h"
    "../../include/CColladaFileLoader.h"
    "../../include/CColladaImage.h"
    "../../include/CColladaInstancedSceneNode.h"
    "../../include/CColladaLightSceneNode.h"
    "../../include/CColladaMaterial.h"
    "../../include/CColladaMesh.h"
    "../../include/CColladaMeshBuffer.h"
    "../../include/CColladaMeshBufferProxy.h"
    "../../include/CColladaMeshBufferRef.h"
    "../../include/CColladaMeshSceneNode.h"
    "../../include/CColladaMeshWriter.h"
    "../../include/CColladaModularSkinnedMesh.h"
    "../../include/CColladaMorphingMesh.h"
    "../../include/CColladaParticleSystemDeflectorSceneNode.h"
    "../../include/CColladaParticleSystemForceSceneNode.h"
    "../../include/CColladaParticleSystemGravitySceneNode.h"
    "../../include/CColladaParticleSystemSceneNode.h"
    "../../include/CColladaParticleSystemWindSceneNode.h"
    "../../include/CColladaResFileManager.h"
    "../../include/CColladaRootSceneNode.h"
    "../../include/CColladaSceneNode.h"
    "../../include/CColladaSceneNodeAnimator.h"
    "../../include/CColladaSceneNodeAnimatorBlender.h"
    "../../include/CColladaSceneNodeAnimatorSet.h"
    "../../include/CColladaSkinnedMesh.h"
    "../../include/CColladaSkinnedMeshSceneNode.h"
    "../../include/CColladaTimelineController.h"
    "../../include/IColladaMesh.h"
    "../../include/IColladaRootSceneNode.h"
    "../../include/IColladaSceneNode.h"
    "../../include/IColladaSceneNodeAnimator.h"
    "../../include/IColladaSkinnedMesh.h"
    "CColladaAnimationTrack.cpp"
    "CColladaAnimationTrackFloat.cpp"
    "CColladaAnimationTrackImageFileList.cpp"
    "CColladaAnimationTrackLightColor.cpp"
    "CColladaAnimationTrackOffsetUV.cpp"
    "CColladaAnimationTrackPosition.cpp"
    "CColladaAnimationTrackQuaternion.cpp"
    "CColladaAnimationTrackTransparency.cpp"
    "CColladaAnimationTrackVector3d.cpp"
    "CColladaAnimationTrackVisibility.cpp"
    "CColladaAnimationTrackWeight.cpp"
    "CColladaBillboardSceneNode.cpp"
    "CColladaCameraSceneNode.cpp"
    "CColladaConverter.cpp"
    "CColladaDatabase.cpp"
    "CColladaDynamicAnimationSet.cpp"
    "CColladaFactory.cpp"
    "CColladaInstancedSceneNode.cpp"
    "CColladaLightSceneNode.cpp"
    "CColladaMaterial.cpp"
    "CColladaMesh.cpp"
    "CColladaMeshBuffer.cpp"
    "CColladaMeshBufferProxy.cpp"
    "CColladaMeshBufferRef.cpp"
    "CColladaMeshSceneNode.cpp"
    "CColladaModularSkinnedMesh.cpp"
    "CColladaModularSkinnedMeshSceneNode.cpp"
    "../../include/CColladaModularSkinnedMeshSceneNode.h"
    "CColladaMorphingMesh.cpp"
    "CColladaParticleSystemSceneNode.cpp"
    "CColladaResFileManager.cpp"
    "CColladaSceneNode.cpp"
    "CColladaSceneNodeAnimator.cpp"
    "CColladaSceneNodeAnimatorBlender.cpp"
    "CColladaSceneNodeAnimatorSet.cpp"
    "CColladaSkinnedMesh.cpp"
    "CColladaSkinnedMeshSceneNode.cpp"
    "CColladaTimelineController.cpp"
    "IColladaMesh.cpp"
    "IColladaSceneNodeAnimator.cpp"
    "IColladaSkinnedMesh.cpp"
)
source_group("scene impl\\collada" FILES ${scene_impl__collada})

set(scene_impl__collision
    "../../include/CMeshCollision.h"
    "CMeshCollision.cpp"
    "CMetaTriangleSelector.cpp"
    "../../include/CMetaTriangleSelector.h"
    "COctTreeTriangleSelector.cpp"
    "../../include/COctTreeTriangleSelector.h"
    "CSceneCollisionManager.cpp"
    "../../include/CSceneCollisionManager.h"
    "CTerrainTriangleSelector.cpp"
    "../../include/CTerrainTriangleSelector.h"
    "CTriangleBBSelector.cpp"
    "../../include/CTriangleBBSelector.h"
    "CTriangleSelector.cpp"
    "../../include/CTriangleSelector.h"
)
source_group("scene impl\\collision" FILES ${scene_impl__collision})

set(scene_impl__loaders
    "C3DSMeshFileLoader.cpp"
    "../../include/C3DSMeshFileLoader.h"
    "CAnimatedMeshMD2.cpp"
    "../../include/CAnimatedMeshMD2.h"
    "CAnimatedMeshMD3.cpp"
    "../../include/CAnimatedMeshMD3.h"
    "CB3DMeshFileLoader.cpp"
    "../../include/CB3DMeshFileLoader.h"
    "CBSPMeshFileLoader.cpp"
    "../../include/CBSPMeshFileLoader.h"
    "CColladaBinaryFileLoader.cpp"
    "../../include/CColladaBinaryFileLoader.h"
    "CColladaFileLoader.cpp"
    "../../include/CColladaFileLoader.h"
    "CCSMLoader.cpp"
    "../../include/CCSMLoader.h"
    "CDMFLoader.cpp"
    "../../include/CDMFLoader.h"
    "CIrrMeshFileLoader.cpp"
    "../../include/CIrrMeshFileLoader.h"
    "CLMTSMeshFileLoader.cpp"
    "../../include/CLMTSMeshFileLoader.h"
    "CLWOMeshFileLoader.cpp"
    "../../include/CLWOMeshFileLoader.h"
    "CMD2MeshFileLoader.cpp"
    "../../include/CMD2MeshFileLoader.h"
    "CMD3MeshFileLoader.cpp"
    "../../include/CMD3MeshFileLoader.h"
    "CMS3DMeshFileLoader.cpp"
    "../../include/CMS3DMeshFileLoader.h"
    "../../include/CMY3DHelper.h"
    "CMY3DMeshFileLoader.cpp"
    "../../include/CMY3DMeshFileLoader.h"
    "../../include/CMY3DStuff.h"
    "COBJMeshFileLoader.cpp"
    "../../include/COBJMeshFileLoader.h"
    "COCTLoader.cpp"
    "../../include/COCTLoader.h"
    "COgreMeshFileLoader.cpp"
    "../../include/COgreMeshFileLoader.h"
    "CQ3LevelMesh.cpp"
    "../../include/CQ3LevelMesh.h"
    "CSkinnedMesh.cpp"
    "../../include/CSkinnedMesh.h"
    "CSkinnedMesh2.cpp"
    "../../include/CSkinnedMesh2.h"
    "CSTLMeshFileLoader.cpp"
    "../../include/CSTLMeshFileLoader.h"
    "CXMeshFileLoader.cpp"
    "../../include/CXMeshFileLoader.h"
    "../../include/dmfsupport.h"
)
source_group("scene impl\\loaders" FILES ${scene_impl__loaders})

set(scene_impl__particleSystem
    "CParticleAnimatedMeshSceneNodeEmitter.cpp"
    "CParticleAttractionAffector.cpp"
    "CParticleBoxEmitter.cpp"
    "../../include/CParticleBoxEmitter.h"
    "CParticleCylinderEmitter.cpp"
    "CParticleFadeOutAffector.cpp"
    "../../include/CParticleFadeOutAffector.h"
    "CParticleGravityAffector.cpp"
    "../../include/CParticleGravityAffector.h"
    "CParticleMeshEmitter.cpp"
    "CParticlePointEmitter.cpp"
    "../../include/CParticlePointEmitter.h"
    "CParticleRingEmitter.cpp"
    "CParticleRotationAffector.cpp"
    "CParticleSizeAffector.cpp"
    "../../include/CParticleSizeAffector.h"
    "CParticleSphereEmitter.cpp"
    "CParticleSpinAffector.cpp"
    "../../include/CParticleSpinAffector.h"
    "CParticleSystemSceneNode.cpp"
    "../../include/CParticleSystemSceneNode.h"
)
source_group("scene impl\\particleSystem" FILES ${scene_impl__particleSystem})

set(scene_impl__sceneNodes
    "CAnimatedMeshSceneNode.cpp"
    "../../include/CAnimatedMeshSceneNode.h"
    "CBillboardSceneNode.cpp"
    "../../include/CBillboardSceneNode.h"
    "CBoneSceneNode.cpp"
    "../../include/CBoneSceneNode.h"
    "CCameraFPSSceneNode.cpp"
    "../../include/CCameraFPSSceneNode.h"
    "CCameraMayaSceneNode.cpp"
    "../../include/CCameraMayaSceneNode.h"
    "CCameraSceneNode.cpp"
    "../../include/CCameraSceneNode.h"
    "CCameraTargetTrackerSceneNode.cpp"
    "../../include/CCameraTargetTrackerSceneNode.h"
    "CCubeSceneNode.cpp"
    "../../include/CCubeSceneNode.h"
    "CDummyTransformationSceneNode.cpp"
    "../../include/CDummyTransformationSceneNode.h"
    "CEmptySceneNode.cpp"
    "../../include/CEmptySceneNode.h"
    "CLightSceneNode.cpp"
    "../../include/CLightSceneNode.h"
    "CMeshSceneNode.cpp"
    "../../include/CMeshSceneNode.h"
    "CMorphingMesh.cpp"
    "../../include/CMorphingMesh.h"
    "COctTreeSceneNode.cpp"
    "../../include/COctTreeSceneNode.h"
    "CQuake3ShaderSceneNode.cpp"
    "../../include/CQuake3ShaderSceneNode.h"
    "CSceneNodeAnimatorCameraFPS.cpp"
    "../../include/CSceneNodeAnimatorCameraFPS.h"
    "CSceneNodeAnimatorCameraMaya.cpp"
    "../../include/CSceneNodeAnimatorCameraMaya.h"
    "CShadowVolumeSceneNode.cpp"
    "../../include/CShadowVolumeSceneNode.h"
    "CSkinnedMeshSceneNode.cpp"
    "../../include/CSkinnedMeshSceneNode.h"
    "CSkyBoxSceneNode.cpp"
    "../../include/CSkyBoxSceneNode.h"
    "CSkyDomeSceneNode.cpp"
    "../../include/CSkyDomeSceneNode.h"
    "CSphereSceneNode.cpp"
    "../../include/CSphereSceneNode.h"
    "CTerrainSceneNode.cpp"
    "../../include/CTerrainSceneNode.h"
    "CTextSceneNode.cpp"
    "../../include/CTextSceneNode.h"
    "CVolumeLightSceneNode.cpp"
    "../../include/CVolumeLightSceneNode.h"
    "CWaterSurfaceSceneNode.cpp"
    "../../include/CWaterSurfaceSceneNode.h"
    "ISceneNode.cpp"
)
source_group("scene impl\\sceneNodes" FILES ${scene_impl__sceneNodes})

set(scene_impl__writers
    "CColladaMeshWriter.cpp"
    "../../include/CColladaMeshWriter.h"
    "CIrrMeshWriter.cpp"
    "../../include/CIrrMeshWriter.h"
    "COBJMeshWriter.cpp"
    "../../include/COBJMeshWriter.h"
    "CSTLMeshWriter.cpp"
    "../../include/CSTLMeshWriter.h"
)
source_group("scene impl\\writers" FILES ${scene_impl__writers})

set(video_impl
    "../../include/CBatchListCompileData.h"
    "../../include/CVideoModeList.h"
    "../../include/ECompileDataType.h"
    "../../include/SProcessBufferBindingBase.h"
    "CBatchListCompileData.cpp"
    "CTextureManager.cpp"
    "CVideoModeList.cpp"
    "IDriverBinding.cpp"
    "IMaterialRenderer.cpp"
    "irrTexGen.cpp"
    "IVideoDriver.cpp"
    "SProcessBufferBindingBase.cpp"
)
source_group("video impl" FILES ${video_impl})

set(video_impl__Direct3D8
    "../../include/CD3D8Driver.h"
    "../../include/CD3D8MaterialRenderer.h"
    "../../include/CD3D8NormalMapRenderer.h"
    "../../include/CD3D8ParallaxMapRenderer.h"
    "../../include/CD3D8ShaderMaterialRenderer.h"
    "../../include/CD3D8Texture.h"
    "CD3D8Driver.cpp"
    "CD3D8NormalMapRenderer.cpp"
    "CD3D8ParallaxMapRenderer.cpp"
    "CD3D8ShaderMaterialRenderer.cpp"
    "CD3D8Texture.cpp"
)
source_group("video impl\\Direct3D8" FILES ${video_impl__Direct3D8})

set(video_impl__Direct3D9
    "CD3D9Driver.cpp"
    "../../include/CD3D9Driver.h"
    "CD3D9HLSLMaterialRenderer.cpp"
    "../../include/CD3D9HLSLMaterialRenderer.h"
    "../../include/CD3D9MaterialRenderer.h"
    "CD3D9NormalMapRenderer.cpp"
    "../../include/CD3D9NormalMapRenderer.h"
    "CD3D9ParallaxMapRenderer.cpp"
    "../../include/CD3D9ParallaxMapRenderer.h"
    "CD3D9ShaderMaterialRenderer.cpp"
    "../../include/CD3D9ShaderMaterialRenderer.h"
    "CD3D9Texture.cpp"
    "../../include/CD3D9Texture.h"
)
source_group("video impl\\Direct3D9" FILES ${video_impl__Direct3D9})

set(video_impl__GL
    "../../include/CCommonGLDriver.h"
    "../../include/CCommonGLExtensionHandler.h"
    "../../include/CCommonGLExtensionList.h"
    "../../include/CCommonGLHeaders.h"
    "../../include/CCommonGLMaterialRenderer.h"
    "../../include/CCommonGLTexture.h"
    "CCommonGLDriver.cpp"
    "CCommonGLExtensionHandler.cpp"
    "CCommonGLMaterialRenderer.cpp"
    "CCommonGLTexture.cpp"
)
source_group("video impl\\GL" FILES ${video_impl__GL})

set(video_impl__GL__OpenGL
    "../../include/COpenGLDriver.h"
    "../../include/COpenGLNormalMapRenderer.h"
    "../../include/COpenGLParallaxMapRenderer.h"
    "../../include/COpenGLShaderMaterialRenderer.h"
    "../../include/COpenGLSLMaterialRenderer.h"
    "../../include/COpenGLTexture.h"
    "../../include/glext.h"
    "COpenGLDriver.cpp"
    "COpenGLNormalMapRenderer.cpp"
    "COpenGLParallaxMapRenderer.cpp"
    "COpenGLShaderMaterialRenderer.cpp"
    "COpenGLSLMaterialRenderer.cpp"
    "COpenGLTexture.cpp"
)
source_group("video impl\\GL\\OpenGL" FILES ${video_impl__GL__OpenGL})

set(video_impl__GL__OpenGLES
    "../../include/COpenGLESDriver.h"
    "../../include/COpenGLESNormalMapRenderer.h"
    "../../include/COpenGLESParallaxMapRenderer.h"
    "../../include/COpenGLESShaderMaterialRenderer.h"
    "../../include/COpenGLESSLMaterialRenderer.h"
    "../../include/COpenGLESTexture.h"
    "COpenGLESDriver.cpp"
    "COpenGLESNormalMapRenderer.cpp"
    "COpenGLESParallaxMapRenderer.cpp"
    "COpenGLESShaderMaterialRenderer.cpp"
    "COpenGLESSLMaterialRenderer.cpp"
    "COpenGLESTexture.cpp"
)
source_group("video impl\\GL\\OpenGLES" FILES ${video_impl__GL__OpenGLES})

set(video_impl__GL__OpenGLES__GLES
    "PowerVR/OGLES/Include/GLES/egl.h"
    "PowerVR/OGLES/Include/GLES/gl.h"
    "PowerVR/OGLES/Include/GLES/glext.h"
)
source_group("video impl\\GL\\OpenGLES\\GLES" FILES ${video_impl__GL__OpenGLES__GLES})

set(video_impl__GL__PS3
    "../../include/CIrrDevicePS3.h"
    "../../include/CPS3Driver.h"
    "../../include/CPS3NormalMapRenderer.h"
    "../../include/CPS3ParallaxMapRenderer.h"
    "../../include/CPS3ShaderMaterialRenderer.h"
    "../../include/CPS3SLMaterialRenderer.h"
    "../../include/CPS3Texture.h"
    "PS3/CIrrDevicePS3.cpp"
    "PS3/CPS3Driver.cpp"
    "PS3/CPS3NormalMapRenderer.cpp"
    "PS3/CPS3ParallaxMapRenderer.cpp"
    "PS3/CPS3ShaderMaterialRenderer.cpp"
    "PS3/CPS3SLMaterialRenderer.cpp"
    "PS3/CPS3Texture.cpp"
)
source_group("video impl\\GL\\PS3" FILES ${video_impl__GL__PS3})

set(video_impl__Null
    "../../include/CColorConverter.h"
    "../../include/CCompressedImage.h"
    "../../include/CFPSCounter.h"
    "../../include/CImage.h"
    "../../include/CImageLoaderBMP.h"
    "../../include/CImageLoaderDDS.h"
    "../../include/CImageLoaderJPG.h"
    "../../include/CImageLoaderPCX.h"
    "../../include/CImageLoaderPNG.h"
    "../../include/CImageLoaderPPM.h"
    "../../include/CImageLoaderPSD.h"
    "../../include/CImageLoaderTGA.h"
    "../../include/CImageLoaderWAL.h"
    "../../include/CNullDriver.h"
    "../../include/IImagePresenter.h"
    "CColorConverter.cpp"
    "CCompressedImage.cpp"
    "CFPSCounter.cpp"
    "CImage.cpp"
    "CImageLoaderBMP.cpp"
    "CImageLoaderDDS.cpp"
    "CImageLoaderJPG.cpp"
    "CImageLoaderPCX.cpp"
    "CImageLoaderPNG.cpp"
    "CImageLoaderPPM.cpp"
    "CImageLoaderPSD.cpp"
    "CImageLoaderTGA.cpp"
    "CImageLoaderWAL.cpp"
    "CNullDriver.cpp"
)
source_group("video impl\\Null" FILES ${video_impl__Null})

set(video_impl__Null__Writer
    "../../include/CImageWriterBMP.h"
    "../../include/CImageWriterJPG.h"
    "../../include/CImageWriterPCX.h"
    "../../include/CImageWriterPNG.h"
    "../../include/CImageWriterPPM.h"
    "../../include/CImageWriterPSD.h"
    "../../include/CImageWriterTGA.h"
    "CImageWriterBMP.cpp"
    "CImageWriterJPG.cpp"
    "CImageWriterPCX.cpp"
    "CImageWriterPNG.cpp"
    "CImageWriterPPM.cpp"
    "CImageWriterPSD.cpp"
    "CImageWriterTGA.cpp"
)
source_group("video impl\\Null\\Writer" FILES ${video_impl__Null__Writer})

set(video_impl__Wii
    "CWiiDriver.cpp"
    "../../include/CWiiDriver.h"
    "Wii/CWiiMaterialRenderer.cpp"
    "Wii/CWiiMaterialRenderer.h"
    "Wii/CWiiTexture.cpp"
    "Wii/CWiiTexture.h"
    "Wii/WiiTextureConv.cpp"
    "Wii/WiiTextureConv.h"
)
source_group("video impl\\Wii" FILES ${video_impl__Wii})

set(wii_impl
    "Wii/CWiiMemMgr.cpp"
    "Wii/CWiiMemMgr.h"
)
source_group("wii impl" FILES ${wii_impl})

set(ALL_FILES
    ${no_group_source_files}
    ${doc}
    ${gui_impl}
    ${include}
    ${include__core}
    ${include__gui}
    ${include__io}
    ${include__scene}
    ${include__video}
    ${io_impl}
    ${irr_impl}
    ${irr_impl__extern__jpeglib}
    ${irr_impl__extern__libpng}
    ${irr_impl__extern__particlesystem}
    ${irr_impl__extern__particlesystem__behaviormodels}
    ${irr_impl__extern__pvrt}
    ${irr_impl__extern__zlib}
    ${scene_impl}
    ${scene_impl__animators}
    ${scene_impl__batching}
    ${scene_impl__collada}
    ${scene_impl__collision}
    ${scene_impl__loaders}
    ${scene_impl__particleSystem}
    ${scene_impl__sceneNodes}
    ${scene_impl__writers}
    ${video_impl}
    ${video_impl__Direct3D8}
    ${video_impl__Direct3D9}
    ${video_impl__GL}
    ${video_impl__GL__OpenGL}
    ${video_impl__GL__OpenGLES}
    ${video_impl__GL__OpenGLES__GLES}
    ${video_impl__GL__PS3}
    ${video_impl__Null}
    ${video_impl__Null__Writer}
    ${video_impl__Wii}
    ${wii_impl}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

#add_precompiled_header(${PROJECT_NAME} "stdafx.h" ".")

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Irrlicht)

################################################################################
# Target name
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    TARGET_NAME_COMMONDEBUG                       "${PROJECT_NAME}.release"
    TARGET_NAME_DEBUGIPHONEEMULATION              "${PROJECT_NAME}_d"
    TARGET_NAME_DEBUGMULTITHREADED                "${PROJECT_NAME}_d"
    TARGET_NAME_DEBUG                             "${PROJECT_NAME}_d"
    TARGET_NAME_RELEASEIPHONEEMULATION            "${PROJECT_NAME}.release"
)
################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_COMMONDEBUG                       "$<CONFIG>"
    OUTPUT_DIRECTORY_COMMONRELEASE                     "$<CONFIG>/"
    OUTPUT_DIRECTORY_COMMONLITEDEBUG                   "$<CONFIG>/"
    OUTPUT_DIRECTORY_COMMONLITERELEASE                 "$<CONFIG>/"
    OUTPUT_DIRECTORY_DEBUGIPHONEEMULATION              "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/Win32-visualstudio"
    OUTPUT_DIRECTORY_DEBUGMULTITHREADEDIRRLICHTEDITION "$<CONFIG>/"
    OUTPUT_DIRECTORY_DEBUGMULTITHREADED                "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/Win32-visualstudio"
    OUTPUT_DIRECTORY_DEBUGSINGLETHREADED               "$<CONFIG>/"
    OUTPUT_DIRECTORY_DEBUG                             "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/Win32-visualstudio"
    OUTPUT_DIRECTORY_RELEASEIPHONEEMULATION            "$<CONFIG>"
    OUTPUT_DIRECTORY_RELEASEMULTITHREADED              "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/Win32-visualstudio/"
    OUTPUT_DIRECTORY_RELEASESINGLETHREADED             "$<CONFIG>/"
    OUTPUT_DIRECTORY_RELEASE                           "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/Win32-visualstudio/"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASEMULTITHREADED              "TRUE"
    INTERPROCEDURAL_OPTIMIZATION_RELEASESINGLETHREADED             "TRUE"
    INTERPROCEDURAL_OPTIMIZATION_RELEASE                           "TRUE"
)
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
    $<$<CONFIG:CommonDebug>:
        MultiThreaded
    >
    $<$<CONFIG:CommonRelease>:
        MultiThreadedDebug
    >
    $<$<CONFIG:CommonLiteDebug>:
        MultiThreadedDebug
    >
    $<$<CONFIG:CommonLiteRelease>:
        MultiThreadedDebug
    >
    $<$<CONFIG:DebugiPhoneEmulation>:
        MultiThreadedDebug
    >
    $<$<CONFIG:DebugMultithreadedIrrLichtEdition>:
        MultiThreadedDebug
    >
    $<$<CONFIG:DebugMultithreaded>:
        MultiThreadedDebug
    >
    $<$<CONFIG:DebugSinglethreaded>:
        MultiThreadedDebug
    >
    $<$<CONFIG:Debug>:
        MultiThreadedDebug
    >
    $<$<CONFIG:ReleaseiPhoneEmulation>:
        MultiThreaded
    >
    $<$<CONFIG:ReleaseMultithreaded>:
        MultiThreadedDLL
    >
    $<$<CONFIG:ReleaseSinglethreaded>:
        MultiThreaded
    >
    $<$<CONFIG:Release>:
        MultiThreadedDLL
    >
    $<$<NOT:$<OR:$<CONFIG:CommonDebug>,$<CONFIG:CommonRelease>,$<CONFIG:CommonLiteDebug>,$<CONFIG:CommonLiteRelease>,$<CONFIG:DebugiPhoneEmulation>,$<CONFIG:DebugMultithreadedIrrLichtEdition>,$<CONFIG:DebugMultithreaded>,$<CONFIG:DebugSinglethreaded>,$<CONFIG:Debug>,$<CONFIG:ReleaseiPhoneEmulation>,$<CONFIG:ReleaseMultithreaded>,$<CONFIG:ReleaseSinglethreaded>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
)
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/zlib;"
    "${CMAKE_CURRENT_SOURCE_DIR}/freetype/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/particlesystem/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/ResFile/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/ColladaConverter/include"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:CommonDebug>:"
        "NDEBUG;"
        "_IRR_IPHONE_EMULATION_;"
        "_CRT_NON_CONFORMING_SWPRINTFS;"
        "_CRT_SECURE_NO_WARNINGS"
    ">"
    "$<$<CONFIG:CommonRelease>:"
        "IRRLICHT_USE_PRECOMPILED_HEADER;"
        "_DEBUG"
    ">"
    "$<$<CONFIG:CommonLiteDebug>:"
        "IRRLICHT_USE_PRECOMPILED_HEADER;"
        "_DEBUG"
    ">"
    "$<$<CONFIG:CommonLiteRelease>:"
        "IRRLICHT_USE_PRECOMPILED_HEADER;"
        "_DEBUG"
    ">"
    "$<$<CONFIG:DebugiPhoneEmulation>:"
        "IRRLICHT_USE_PRECOMPILED_HEADER;"
        "_DEBUG"
    ">"
    "$<$<CONFIG:DebugMultithreadedIrrLichtEdition>:"
        "IRRLICHT_USE_PRECOMPILED_HEADER;"
        "_DEBUG"
    ">"
    "$<$<CONFIG:DebugMultithreaded>:"
        "IRRLICHT_USE_PRECOMPILED_HEADER;"
        "_DEBUG"
    ">"
    "$<$<CONFIG:DebugSinglethreaded>:"
        "IRRLICHT_USE_PRECOMPILED_HEADER;"
        "_DEBUG"
    ">"
    "$<$<CONFIG:Debug>:"
        "IRRLICHT_USE_PRECOMPILED_HEADER;"
        "_DEBUG"
    ">"
    "$<$<CONFIG:ReleaseiPhoneEmulation>:"
        "NDEBUG;"
        "_IRR_IPHONE_EMULATION_;"
        "_CRT_NON_CONFORMING_SWPRINTFS;"
        "_CRT_SECURE_NO_WARNINGS"
    ">"
    "$<$<CONFIG:ReleaseMultithreaded>:"
        "NDEBUG;"
        "_MBCS"
    ">"
    "$<$<CONFIG:ReleaseSinglethreaded>:"
        "NDEBUG;"
        "_MBCS"
    ">"
    "$<$<CONFIG:Release>:"
        "NDEBUG;"
        "_MBCS"
    ">"
    "WIN32;"
    "_WINDOWS;"
    "_USRDLL;"
    "_CRT_SECURE_NO_DEPRECATE;"
    "_IRR_STATIC_LIB_"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:CommonDebug>:
            /Od;
            /RTC1;
            /Zi;
            /GR-;
            /MP
        >
        $<$<CONFIG:CommonRelease>:
            /Od;
            /RTC1;
            /Zi;
            /GR;
            /wd4250;
            /MP
        >
        $<$<CONFIG:CommonLiteDebug>:
            /Od;
            /RTC1;
            /Zi;
            /GR;
            /wd4250;
            /MP
        >
        $<$<CONFIG:CommonLiteRelease>:
            /Od;
            /RTC1;
            /Zi;
            /GR;
            /wd4250;
            /MP
        >
        $<$<CONFIG:DebugiPhoneEmulation>:
            /Od;
            /RTC1;
            /Zi;
            /GR;
            /wd4250;
            /MP
        >
        $<$<CONFIG:DebugMultithreadedIrrLichtEdition>:
            /Od;
            /RTC1;
            /Zi;
            /GR;
            /wd4250;
            /MP
        >
        $<$<CONFIG:DebugMultithreaded>:
            /Od;
            /RTC1;
            /Zi;
            /GR;
            /wd4250;
            /MP
        >
        $<$<CONFIG:DebugSinglethreaded>:
            /Od;
            /RTC1;
            /Zi;
            /GR;
            /wd4244;
            /wd4996;
            /wd4530;
            /MP
        >
        $<$<CONFIG:Debug>:
            /Od;
            /RTC1;
            /Zi;
            /GR;
            /wd4250;
            /MP
        >
        $<$<CONFIG:ReleaseiPhoneEmulation>:
            /Od;
            /RTC1;
            /Zi;
            /GR-;
            /MP
        >
        $<$<CONFIG:ReleaseMultithreaded>:
            /O2;
            /Ob1;
            /Oi-;
            /GF;
            /Gy;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /GS-
        >
        $<$<CONFIG:ReleaseSinglethreaded>:
            /O2;
            /Ob1;
            /Oi-;
            /GF;
            /Gy;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /GS-
        >
        $<$<CONFIG:Release>:
            /O2;
            /Ob1;
            /Oi-;
            /GF;
            /Gy;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /GS-
        >
        /W3;
        /nologo;
        /MP;
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/cdjpeg.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jcapimin.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jcapistd.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jccoefct.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jccolor.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jcdctmgr.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jchuff.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jcinit.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jcmainct.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jcmarker.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jcmaster.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jcomapi.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jcparam.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jcphuff.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jcprepct.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jcsample.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jctrans.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jdapimin.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jdapistd.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jdatadst.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jdatasrc.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jdcoefct.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jdcolor.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jddctmgr.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jdhuff.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jdinput.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jdmainct.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jdmarker.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jdmaster.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jdmerge.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jdphuff.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jdpostct.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jdsample.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jdtrans.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jerror.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jfdctflt.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jfdctfst.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jfdctint.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jidctflt.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jidctfst.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jidctint.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jidctred.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jmemirr.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jmemmgr.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jquant1.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jquant2.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(jpeglib/jutils.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(libpng/png.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(libpng/pngerror.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(libpng/pngget.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(libpng/pngmem.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(libpng/pngpread.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(libpng/pngread.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(libpng/pngrio.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(libpng/pngrtran.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(libpng/pngrutil.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(libpng/pngset.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(libpng/pngtrans.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(libpng/pngwio.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(libpng/pngwrite.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(libpng/pngwtran.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(libpng/pngwutil.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(zlib/adler32.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(zlib/compress.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(zlib/crc32.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(zlib/deflate.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(zlib/inffast.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(zlib/inflate.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(zlib/inftrees.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(zlib/trees.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(zlib/uncompr.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(zlib/zutil.c ${FILE_CL_OPTIONS})
endif()

################################################################################
# Pre build events
################################################################################
add_custom_command_if(
    TARGET ${PROJECT_NAME}
    PRE_BUILD
    COMMANDS
    COMMAND                       $<CONFIG:CommonDebug> $<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\..\\tools\\setupCompileConfig.bat
    COMMAND                     $<CONFIG:CommonRelease> $<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\..\\tools\\setupCompileConfig.bat
    COMMAND                   $<CONFIG:CommonLiteDebug> $<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\..\\tools\\setupCompileConfig.bat
    COMMAND                 $<CONFIG:CommonLiteRelease> $<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\..\\tools\\setupCompileConfig.bat
    COMMAND              $<CONFIG:DebugiPhoneEmulation> $<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\..\\tools\\setupCompileConfig.bat
    COMMAND $<CONFIG:DebugMultithreadedIrrLichtEdition> $<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\..\\tools\\setupCompileConfig.bat
    COMMAND                $<CONFIG:DebugMultithreaded> $<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\..\\tools\\setupCompileConfig.bat
    COMMAND               $<CONFIG:DebugSinglethreaded> $<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\..\\tools\\setupCompileConfig.bat
    COMMAND                             $<CONFIG:Debug> $<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\..\\tools\\setupCompileConfig.bat
    COMMAND            $<CONFIG:ReleaseiPhoneEmulation> $<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\..\\tools\\setupCompileConfig.bat
    COMMAND              $<CONFIG:ReleaseMultithreaded> $<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\..\\tools\\setupCompileConfig.bat
    COMMAND             $<CONFIG:ReleaseSinglethreaded> $<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\..\\tools\\setupCompileConfig.bat
    COMMAND                           $<CONFIG:Release> $<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\..\\tools\\setupCompileConfig.bat
)

################################################################################
# Dependencies
################################################################################
set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "$<$<CONFIG:CommonDebug>:"
        "libgles_cm"
    ">"
    "$<$<CONFIG:ReleaseiPhoneEmulation>:"
        "libgles_cm"
    ">"
)
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

target_link_directories(${PROJECT_NAME} PUBLIC
    "$<$<CONFIG:CommonDebug>:"
        "${CMAKE_CURRENT_SOURCE_DIR}//freetype/objs;"
        "${CMAKE_CURRENT_SOURCE_DIR}/PowerVR/OGLES/WindowsPC/Lib"
    ">"
    "$<$<CONFIG:ReleaseiPhoneEmulation>:"
        "${CMAKE_CURRENT_SOURCE_DIR}//freetype/objs;"
        "${CMAKE_CURRENT_SOURCE_DIR}/PowerVR/OGLES/WindowsPC/Lib"
    ">"
)


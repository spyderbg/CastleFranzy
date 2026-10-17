#include "stdafx.h"
#include "Misc.h"
#include "Game/Config/GameConfig.h"
#include "IColladaSceneNodeAnimator.h"
#include <map>
#include <algorithm>
#include <string>

void cleanNode (ISceneNode * node, bool unit, bool ref)
{
	list<ISceneNode*>::ConstIterator it = node->getChildren().begin();
	for (; it != node->getChildren().end();)
	{
		ISceneNode * child = (*it);
		it++;
		cleanNode(child, unit); 
	}

	if ((strstr(node->getName(),"_PIVOT") && node->getChildren().begin() == node->getChildren().end()) ||
		(node->getType() == ESNT_LIGHT) )
	{
		node->remove();
		return;
	}

	for (int i = 0; i < node->getMaterialCount(); ++i)
	{
		SMaterial& material = node->getMaterial(i);

		//material.setTexture(0,0);
		material.setFlag(EMF_LIGHTING, true);
		material.setFlag(EMF_NORMALIZE_NORMALS, true);
		material.setEmissiveColor(0x00000000);
		material.setShininess(0.0f);

		if (unit || ref)
		{
			material.setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL_REF);
			if (unit)
			{
				material.setFlag(EMF_BACK_FACE_CULLING, true);
			}
		}

		if (strstr(node->getUID(), "light"))
		{
			material.setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
		}
		if (material.getMaterialType() == EMT_SOLID)
		{
			material.setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL_REF);
		}
	}
}

void setEmissiveColor(ISceneNode* node, SColor color)
{
	list<ISceneNode*>::ConstIterator it = node->getChildren().begin();
	for (; it != node->getChildren().end();)
	{
		ISceneNode * child = (*it);
		it++;
		setEmissiveColor(child, color); 
	}

	for (int i = 0; i < node->getMaterialCount(); ++i)
	{
		node->getMaterial(i).setEmissiveColor(color);
	}
}

position2di screenFrom3DPos (const vector3df& pos3d)
{
	return __IRR_SCENE()->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(pos3d);
}

vector2df vector2DFrom3DPos (const vector3df& pos3d)
{
	position2di pos = screenFrom3DPos(pos3d);
	return vector2df(pos.X, pos.Y);
}

float GetFloatWithDefault (IAttributes* attr, const char* name, float default_val)
{
	int index = attr->findAttribute(name);
	if (index >= 0)
	{
		return attr->getAttributeAsFloat(index);
	}
	else
	{
		return default_val;
	}
}

int RandInt (int min, int max) // inclusive
{
	return min + (rand() % (max - min + 1));
}

float RandFloat (float min, float max)
{
	return (rand() / float(RAND_MAX)) * (max - min) + min;
}

int IntRemap1 (int x, int f0, int f1, int t0, int t1)
{
	int64 tw = t1 - t0;
	int64 fw = f1 - f0;
	if (fw == 0)
	{
		__IRR_DEVICE()->getLogger()->log("IntRemap1 called with empty interval!", ELL_WARNING);
		return (t0 + t1) / 2;
	}
	return (s32(t0 + (fw + int64(2) * (int64(x) - f0) * tw) / (int64(2) * fw)));
}

int IntRemap2 (int x, int f0, int f1, int t0, int t1)
{
	int64 tw = t1 - t0 + 1;
	int64 fw = f1 - f0 + 1;
	return (s32(t0 + (int64(2) * (int64(x) - f0) + int64(1)) * tw / (int64(2) * fw)));
}

int IntRemap3 (int x, int f0, int f1, int t0, int t1)
{
	if (f0 == f1) return t0;

	if (x < (f0 + f1) / 2)
		return t0 + (t1 - t0 + 1) * (x - f0) / (f1 - f0);
	else
		return t1 - (t1 - t0 + 1) * (f1 - x) / (f1 - f0);
}


float FloatRemap (float x, float f0, float f1, float t0, float t1)
{
	if (f1 - f0 == 0.0f)
	{
		__IRR_DEVICE()->getLogger()->log("FloatRemap called with empty interval!", ELL_WARNING);
		return (t0 + t1) / 2.0f;
	}
	return t0 + (x - f0) / (f1 - f0) * (t1 - t0);
}

// looks like this:
// 1            *-------------*
//             /               \
//            /                 \
// 0 --------*                   *--------
//          x1  x2           x3  x4
float LinearUpFlatDown (float x, float x1, float x2, float x3, float x4)
{
	if (x < x1) return 0.0f;
	if (x < x2) return (x - x1) / (x2 - x1);
	if (x < x3) return 1.0f;
	if (x < x4) return (x4 - x) / (x4 - x3);
	return 0.0f;
}

// looks like this:
//                           /
// +1                       *
//                         /x4
//                        /
//                       /
//  0       *-----------*
//         /x2          x3
//        /
//       /
// -1   *
//     /x1          
float LinearUpFlatUp (float x, float x1, float x2, float x3, float x4)
{
	if (x < x2) return (x - x2) / (x2 - x1);
	if (x < x3) return 0.0f;
	return (x - x3) / (x4 - x3);
}

// looks like this:
// +1                  *------------
//                    /x2
//                   /
//                  /
//  0 -------------*
//                 x1
float LinearStep (float x, float x1, float x2)
{
	if (x < x1) return 0.0f;
	if (x < x2) return (x - x1) / (x2 - x1);
	return 1.0f;
}

vector3df randomVectorInSphere (float radius)
{
	vector3df result;
	do
	{
		result.X = RandFloat(-1.0f, 1.0f);
		result.Y = RandFloat(-1.0f, 1.0f);
		result.Z = RandFloat(-1.0f, 1.0f);
	}
	while (result.dotProduct(result) > 1.0f);
	return result * radius;
}

void RemoveUnusedTextures ()
{
	IVideoDriver* driver = __IRR_DRIVER();
	for (int i = driver->getTextureCount() - 1; i >= 0; --i)
	{
		ITexture* texture = driver->getTextureByIndex(i);
		if (texture->getReferenceCount() == 1)
		{
			DBG_OUT("(%3d) Unloading texture: %s\n", driver->getTextureCount(), texture->getName().c_str());
			driver->removeTexture(texture);
		}
	}
}

v2f HermiteInterpolation (float t, v2f p1, v2f p2, v2f m1, v2f m2)
{
	return
		p1 * ((1 + 2 * t) * (1 - t) * (1 - t)) +
		m1 * (t * (1 - t) * (1 - t)) +
		p2 * (t * t * (3 - 2 * t)) +
		m2 * (t * t * (t - 1));
}

v2f HermiteInterpolationDerivative (float t, v2f p1, v2f p2, v2f m1, v2f m2)
{
	return
		p1 * (6 * (t - 1) * t) -
		p2 * (6 * (t - 1) * t) +
		m2 * (t * (3 * t - 2)) +
		m1 * ((t - 1) * (3 * t - 1));
}

v2f HermiteInterpolationSecondDerivative (float t, v2f p1, v2f p2, v2f m1, v2f m2)
{
	return m1 * (6 * t - 4) + m2 * (6 * t - 2) + 6 * (2 * t - 1) * (p1 - p2);
}

v2f HermiteInterpolationOffset (float t, float offset, v2f p1, v2f p2, v2f m1, v2f m2)
{
	v2f x = HermiteInterpolation(t, p1, p2, m1, m2);
	v2f dx = HermiteInterpolationDerivative(t, p1, p2, m1, m2);
	v2f n(-dx.Y, dx.X);
	return x + (offset / n.getLength()) * n;
}

float HermiteInterpolationCurvature (float t, v2f p1, v2f p2, v2f m1, v2f m2)
{
	v2f dx = HermiteInterpolationDerivative(t, p1, p2, m1, m2);
	v2f ddx = HermiteInterpolationSecondDerivative(t, p1, p2, m1, m2);
	v2f pddx(-ddx.Y, ddx.X);
	return dx.dotProduct(pddx) * powf(dx.dotProduct(dx), -1.5f);
}


// --- collada scenes file cache ---
static std::map<std::string, IReadFile*> s_files;
static std::map<IReadFile*, std::string> s_strings;
static std::list<IReadFile*> s_deleteList;
static int s_cacheSize = 0;
const int k_maxCacheSize = 3 * 1024 * 1024;

void CleanColladaCache (bool wholeCache)
{
	bool printSize = false;
	while (s_cacheSize > k_maxCacheSize || (wholeCache && !s_deleteList.empty()))
	{
		printSize = true;
		IReadFile* file = s_deleteList.front();
		std::string str = s_strings[file];
		s_cacheSize -= file->getSize();
		file->drop();
		s_files.erase(str);
		s_strings.erase(file);
		s_deleteList.pop_front();

		DBG_OUT("removed file from cache: %s\n", str.c_str());
	}

	if (printSize)
		DBG_OUT("cache size: %d k\n", s_cacheSize / 1024);
}

void PreLoadColladaScene(const char* filename)
{
	if (s_files.find(filename) == s_files.end())
	{
		IReadFile* file = __IRR_FILE_SYSTEM()->createAndOpenFile(filename);
		int size = file->getSize();
		u8* buffer = new u8[size];
		file->read(buffer, size);
		file->drop();
		file = __IRR_FILE_SYSTEM()->createMemoryReadFile(buffer, size, filename, true);

		s_files[filename] = file;
		s_strings[file] = filename;
		s_deleteList.push_back(file);

		s_cacheSize += size;

		DBG_OUT("new file in cache: %s\n", filename);
		DBG_OUT("cache size: %d k\n", s_cacheSize / 1024);
	}
}

ISceneNode* LoadColladaScene (const char* filename, bool cache)
{
	if (cache)
		PreLoadColladaScene(filename);

	ISceneNode* result;

	IReadFile* file = s_files[filename];
	if (file)
	{
		file->seek(0);
		result = __IRR_SCENE()->loadColladaScene(file);

		s_deleteList.erase(std::find(s_deleteList.begin(), s_deleteList.end(), file));
		s_deleteList.push_back(file);

		if (cache)
			CleanColladaCache(false);
	}
	else
	{
		result = __IRR_SCENE()->loadColladaScene(filename);
	}

	if (result && !result->getAnimators().empty())
	{
		((collada::ISceneNodeAnimator*)result->getAnimators().getFirstElem())->setInterpolationMode(collada::ISceneNodeAnimator::EIT_STEP);
	}

	return result;
}


// ---- frame stats ----

const int k_maxFrames = 3600;
const int k_maxEvents = 1000;

int s_frame = 0;
int s_event = 0;
static int s_frameTimes[k_maxFrames] = { 0 };
static FrameEvent s_frameEvents[k_maxEvents] = { 0 };

void FrameEnded (int ms)
{
	s_frameTimes[s_frame++ % k_maxFrames] = ms;
}

void AddFrameEvent (int type, int data)
{
	int index = s_event++ % k_maxEvents;
	s_frameEvents[index].frame = s_frame;
	s_frameEvents[index].type = type;
	s_frameEvents[index].data = data;
}

void SaveFrameStats ()
{
	IWriteFile* file;
	stringc path, txt;
	
	path = __APP()->getFileSavePath();
	path += "frames.txt";
	file = __IRR_FILE_SYSTEM()->createAndWriteFile(path.c_str());
	while (!file)
		file = __IRR_FILE_SYSTEM()->createAndWriteFile(path.c_str());
	if (file)
	{
		txt = "frames\n";
		file->write(txt.c_str(), txt.size());

		for (int frame = MAX(0, s_frame - k_maxFrames); frame < s_frame; ++frame)
		{
			txt = stringc(frame) + "," + stringc(s_frameTimes[frame % k_maxFrames]) + stringc("\n");
			file->write(txt.c_str(), txt.size());
		}

		txt = "\nevents\n";
		file->write(txt.c_str(), txt.size());
		
		for (int ev = MAX(0, s_event - k_maxEvents); ev < s_event; ++ev)
		{
			FrameEvent fe = s_frameEvents[ev % k_maxEvents];
			txt = stringc(fe.frame) + stringc(",") + FrameEventNames[fe.type] + stringc(",") + stringc(fe.data) + stringc("\n");
			file->write(txt.c_str(), txt.size());
		}
		file->drop();
	}
}

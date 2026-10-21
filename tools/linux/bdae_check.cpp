// Exercise BRES relocation, animation binding and scene construction in the actual game engine.
#include "irrlicht.h"
#include "CColladaDatabase.h"
#include "CColladaResFileManager.h"
#include "CColladaSceneNodeAnimator.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

void* IrrAlloc(size_t size, irr::MemHint) { return std::malloc(size); }
void* IrrAlloc(size_t size, irr::MemHint hint, int, const char*, int) { return IrrAlloc(size, hint); }
void IrrFree(void* pointer) { std::free(pointer); }

int main(int argc, char** argv) {
    std::string textureRoot;
    bool geometryOnly = false;
    if (argc < 2) return 2;
    for (int i = 2; i < argc; ++i) {
        if (std::string(argv[i]) == "--geometry-only") geometryOnly = true;
        else if (std::string(argv[i]) == "--texture-root" && i+1 < argc) textureRoot = argv[++i];
        else { std::fprintf(stderr, "Usage: BdaeCheck model.bdae [--texture-root directory] [--geometry-only]\n"); return 2; }
    }
    irr::IrrlichtDevice* device = irr::createDevice(irr::video::EDT_NULL);
    if (!device) return 1;
    int result = 0;
    {
        irr::collada::CResFileManager manager(device);
        irr::collada::CResFile* resource = manager.get(argv[1]);
        if (!resource || !resource->isValid()) return 1;
        irr::collada::CColladaDatabase database(resource);
        const irr::collada::SCollada& data = *static_cast<const irr::collada::SCollada*>(resource->getDataPtr());
        std::string directory(argv[1]);
        const size_t slash = directory.find_last_of("/\\");
        directory = slash == std::string::npos ? "" : directory.substr(0, slash+1);
        for (int i = 0; !geometryOnly && i < data.libraryImages.images.size(); ++i) {
            const std::string name(data.libraryImages.images[i].path.ptr);
            std::vector<std::string> candidates;
            candidates.push_back(directory+name);
            if (!textureRoot.empty()) candidates.push_back(textureRoot+"/"+name);
            const size_t count = candidates.size();
            for (size_t k = 0; k < count; ++k) {
                const size_t dot = candidates[k].find_last_of('.');
                if (dot != std::string::npos) {
                    candidates.push_back(candidates[k].substr(0,dot)+".png");
                    candidates.push_back(candidates[k].substr(0,dot)+".tga");
                }
            }
            irr::video::IImage* image = 0;
            for (size_t k = 0; k < candidates.size() && !image; ++k) {
                std::FILE* file = std::fopen(candidates[k].c_str(), "rb");
                if (file) { std::fclose(file); image = device->getVideoDriver()->createImageFromFile(candidates[k].c_str()); }
            }
            if (!image) { std::fprintf(stderr, "Image decoding failed\n"); return 1; }
            std::printf("TEXTURE %s %u %u\n", data.libraryImages.images[i].path.ptr.ptr(),
                        image->getDimension().Width, image->getDimension().Height);
            image->drop();
        }
        for (int i = 0; i < database.getAnimationCount(); ++i) {
            const irr::collada::SAnimation* animation = database.getAnimation(i);
            if (!animation->getKeyTime().size() || !animation->getOutput().size()) return 1;
            for (int k = 0; k < animation->getOutput().size(); ++k)
                if (!std::isfinite(animation->getOutput()[k])) return 1;
        }
        irr::scene::ISceneNode* root = database.constructScene(true);
        if (!root) return 1;
        irr::collada::CSceneNodeAnimator* animator =
            root->getAnimators().getSize() ? static_cast<irr::collada::CSceneNodeAnimator*>(root->getAnimators().getFirstElem()) : 0;
        for (int i = 0; i < database.getAnimationClipCount(); ++i) {
            const irr::collada::SAnimationClip* clip = database.getAnimationClip(i);
            if (clip->end < clip->start) return 1;
            if (animator) animator->getTimelineCtrl()->setClip(i);
            for (int sample = 0; animator && sample <= 4; ++sample) {
                animator->getTimelineCtrl()->jumpTo(clip->start+(clip->end-clip->start)*sample/4);
                animator->applyAnimationValues(0);
                root->updateAbsolutePosition(true);
            }
            std::printf("CLIP %s %d %d\n", clip->id.ptr.ptr(), clip->start, clip->end);
        }
        for (int i = 0; i < database.getGeometryCount(); ++i) {
            const irr::collada::SGeometry* geometry = database.getGeometry(i);
            if (geometry->type != irr::collada::SGeometry::EGT_MESH) continue;
            int triangles = 0;
            for (int b = 0; b < geometry->pMesh->meshBuffers.size(); ++b)
                triangles += geometry->pMesh->meshBuffers[b].count;
            std::printf("MESH %s %d vertices %d triangles\n", geometry->name.ptr.ptr(),
                        geometry->pMesh->getVertexCount(), triangles);
        }
        std::printf("BDAE_NATIVE_OK %d tracks %d clips %d events\n", database.getAnimationCount(),
                    database.getAnimationClipCount(), data.libraryAnimations.eventsTrack ? data.libraryAnimations.eventsTrack->times.iData.size() : 0);
        root->drop();
    }
    device->drop();
    return result;
}

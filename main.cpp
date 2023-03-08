#include "threepp/extras/imgui/imgui_context.hpp"
#include "threepp/threepp.hpp"
#include <cmath>
#include <memory>
bool startSelect = false;
using namespace threepp;
namespace {

    struct MyListener: MouseListener {

        float &t;

        explicit MyListener(float &t) : t(t) {}

        void onMouseDown(int button, const Vector2& pos) override {
            std::cout << "onMouseDown, button= " << button << ", pos=" << pos << " at t=" << t << std::endl;
        }

        void onMouseUp(int button, const Vector2& pos) override {
            if(button == 0){
                startSelect = true;
            }


            std::cout << "onMouseUp, button= " << button << ", pos=" << pos << " at t=" << t << std::endl;
        }

        void onMouseMove(const Vector2& pos) override {
            //std::cout << "onMouseMove, " << "pos=" << pos << " at t=" << t << std::endl;
        }

        void onMouseWheel(const Vector2& delta) override {
            std::cout << "onMouseWheel, " << "delta=" << delta << " at t=" << t << std::endl;
        }
    };

}


int main() {
    Canvas canvas;
    GLRenderer renderer(canvas);

    auto camera = PerspectiveCamera::create(75.0,1,0.1,3000);
    camera->position.y = 10;
    camera->position.z = 5;

    OrbitControls controls{camera, canvas};

    auto scene = Scene::create();
    scene->background->setRGB(81,89,95);

    // Import and load obj files for the scene
    OBJLoader loader;
    auto table = loader.load("bin/data/testBoard1.obj", false);
    //auto tableMat = MeshBasicMaterial::create();
    //tableMat->color = Color(255,255,255);
    //auto tableMesh = Mesh::create(table, tableMat);

    scene->add(table);

    // add light to scene
    auto light = HemisphereLight::create(0xffffff, 0x888888);
    light->position.set(0,1,0);
    scene->add(light);


    auto collisionObjects = Group::create();

    // For loop for generating 64 planes in a chesslike formation,
    // these are used for collision in the program, and for showing
    // what moves are possible to make when selecting a piece.
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            auto plane = PlaneGeometry::create(1,1);
            auto material = MeshBasicMaterial::create();
            auto mesh = Mesh::create(plane, material);
            // Need to change rotation because of world coordinate system
            auto axis = Vector3(1, 0, 0); // Rotate around the Y-axis
            auto angle = -M_PI / 2; // Rotate 90 degrees
            mesh->rotateOnAxis(axis, angle);

            // x = 1, y = 1 -> Black
            // x = 2, y = 1 -> White
            if(i%2 == 0) {
                if (j%2 != 0) {
                    material->color = Color(0x000000);
                }
            } else {
                if (j%2 == 0){
                    material->color = Color(0x000000);
                }
            }
            mesh->position.set(j-3.5,0,i-3.5);
            scene->add(mesh);
        }
    }
    //scene->add(collisionObjects);


    canvas.onWindowResize([&](WindowSize size) {
        camera->aspect = size.getAspect();
        camera->updateProjectionMatrix();
        renderer.setSize(size);
    });

    // Mouse listener to normalize input
    Vector2 mouse{-Infinity<float>, -Infinity<float>};
    MouseMoveListener mouseListener([&](auto &pos) {
        auto size = canvas.getSize();
        mouse.x = (pos.x / static_cast<float>(size.width)) * 2 - 1;
        mouse.y = -(pos.y / static_cast<float>(size.height)) * 2 + 1;
    });

    float t = 0;
    MyListener l{t};


    canvas.addMouseListener(&l);

    canvas.addMouseListener(&mouseListener);


    Raycaster raycaster;
    // Renderer / Animation
    canvas.animate([&](float dt) {
        t += dt;
        if(startSelect) {


        raycaster.setFromCamera(mouse, camera.get());
        // TODO: This needs to be changed to collision group when you have time
        auto intersected = raycaster.intersectObjects(scene->children);
        if(intersected.size() > 0) {
            auto hasSelected = MeshBasicMaterial::create();
            hasSelected->color = Color(20,50,22);
            auto prevObj = scene->getObjectByName("selected");
            if (prevObj->is<Mesh>()){
                prevObj->as<Mesh>()->setMaterial(hasSelected);
                prevObj->name = "";
            }

            auto &intersect = intersected.front();
            auto obj = intersect.object;
            obj->name = "selected";

            auto newMat = MeshBasicMaterial::create();
            newMat->color = Color(0,255,128);
            auto orgMat = obj->as<Mesh>()->material()->clone();
            if (obj->is<Mesh>()) {

                obj->as<Mesh>()->setMaterial(newMat);
            }

            if(intersected.size() == 0){
                obj->as<Mesh>()->setMaterial(orgMat);
            }
        }
        startSelect = false;
        }
        renderer.render(scene, camera);
    });
}

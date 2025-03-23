#include "game.h"

Game gaming;

int __stdcall WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd) {

    fpsController fpsC;
    initgraph(graphW, graphH, EX_DBLCLKS);

    BeginBatchDraw();

    fpsC.set(gaming.fps);

    while (true) {
        fpsC.start();

        gaming.progress(fpsC);

        gaming.move();
        if (gaming.running) {
            gaming.upgrade();
        }

        cleardevice();
        gaming.draw();
        fpsC.draw();
        FlushBatchDraw();

        fpsC.delay();
    }

    EndBatchDraw();

    return 0;
}
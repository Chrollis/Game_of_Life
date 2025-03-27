#include "game.h"

Game gaming;

int __stdcall WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd) {
    clock_t last_tick = clock();
    clock_t current_tick = clock();
    ExMessage msg;

    initgraph(graph_side_length, graph_side_length);
    gaming.enter();
    gaming.fps_controller.set(12);

    BeginBatchDraw();

    while (1) {
        gaming.fps_controller.start();

        while (peekmessage(&msg)) {
            gaming.input(msg);
        }

        current_tick = clock();
        gaming.proceed(current_tick - last_tick);
        last_tick = current_tick;

        cleardevice();
        gaming.draw();
        gaming.fps_controller.draw();

        FlushBatchDraw();

        gaming.fps_controller.delay();
    }

    EndBatchDraw();

    
    closegraph();
    return 0;
}
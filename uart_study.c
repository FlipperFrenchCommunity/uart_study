/*
 * Project: Apprentissage de l'UART
 * File: uart_study.c
 * Create by: Rom1 <rom1@canel.ch> - Flipper French Community - https://github.com/FlipperFrenchCommunity
 * Date: 26 octobre 2022
 * License: GNU GENERAL PUBLIC LICENSE v3
 * Description: Découvrir le protocole UART
 */
#include <furi.h>
#include <furi_hal_uart.h>
#include <furi_hal_console.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>


typedef struct {
    FuriMessageQueue* event_queue;

    FuriMutex* model_mutex;

    ViewPort* view_port;
    Gui* gui;
} UartStudyState;

void uart_study_draw_callback(Canvas* canvas, void* ctx){
    UNUSED(canvas);
    UartStudyState* state = ctx;
    furi_check(furi_mutex_acquire(state->model_mutex, 25) == FuriStatusOk);

    furi_mutex_release(state->model_mutex);
}

void uart_study_input_callback(InputEvent* input, void* ctx){
    UartStudyState* state = ctx;
    furi_check(furi_mutex_acquire(state->model_mutex, 25) == FuriStatusOk);
    furi_message_queue_put(state->event_queue, input, FuriWaitForever);
    furi_mutex_release(state->model_mutex);
}

UartStudyState* uart_study_alloc(){
    UartStudyState* state = malloc(sizeof(UartStudyState));

    state->view_port = view_port_alloc();
    state->model_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    state->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(state->gui, state->view_port, GuiLayerFullscreen);

    state->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    
    view_port_draw_callback_set(state->view_port, uart_study_draw_callback, state);
    view_port_input_callback_set(state->view_port, uart_study_input_callback, state);
    
    return state;
}

void uart_study_free(UartStudyState* state){
    view_port_enabled_set(state->view_port, false);
    gui_remove_view_port(state->gui, state->view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(state->view_port);

    furi_message_queue_free(state->event_queue);
    furi_mutex_free(state->model_mutex);

    free(state);
}

int32_t uart_study_app(void* p){
    UNUSED(p);

    UartStudyState* state = uart_study_alloc();

    furi_hal_console_disable();
    furi_hal_uart_set_br(FuriHalUartIdUSART1, 115200);
    uint8_t data[4]
    data[0] = 65;
    data[1] = 66;
    data[2] = 67;
    data[3] = 68;

    InputEvent event;
    for(bool processing=true ; processing ; ){
        FuriStatus event_status = furi_message_queue_get(state->event_queue,
        &event, 100);

        furi_check(furi_mutex_acquire(state->model_mutex, 25) == FuriStatusOk);

        if(event_status == FuriStatusOk) {
            if(event.type == InputTypePress) {
                switch(event.key) {
                    case InputKeyUp:
                    case InputKeyDown:
                    case InputKeyLeft:
                    case InputKeyRight:
                    case InputKeyOk:
                        furi_hal_uart_tx(FuriHalUartIdUSART1, data, 4);
                        break;
                    case InputKeyBack:
                        processing = false;
                        break;
                }
            }
        }
        furi_mutex_release(state->model_mutex);
        view_port_update(state->view_port);
    }

    furi_hal_console_enable();

    uart_study_free(state);
    return 0;
}

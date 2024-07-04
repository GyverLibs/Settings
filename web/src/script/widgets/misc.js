import { Component } from "@alexgyver/component";

export function intToColor(int) {
    return "#" + Number(int).toString(16).padStart(6, '0');
}

export function Arrow(dir = 'right', size = 20) {
    return Component.make('div', {
        class: 'arrow_cont',
        style: {
            width: (size * 0.7) + 'px',
            height: size + 'px',
        },
        child: {
            tag: 'div',
            class: 'arrow ' + dir,
        }
    });
}
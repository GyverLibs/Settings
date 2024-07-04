import { Component } from "@alexgyver/component";

export default function popup(text) {
    let popup = Component.make('div', {
        class: 'popup',
        style: {
            animation: 'fader 2.5s',
        },
        text: text,
        parent: document.getElementsByClassName('popup_cont')[0],
    });
    setTimeout(() => popup.remove(), 2500);
    console.log(text);
}
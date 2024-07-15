import { Component } from "@alexgyver/component";
import Button from "./button";

export default class ButtonsWidget {
    $root;
    buttons = [];

    constructor(data) {
        for (let b of data.content) {
            if (b.type == 'button') this.buttons.push(new Button(b));
        }

        this.$root = Component.make('div', {
            class: 'buttons',
            children: this.buttons.map(b => b.$root),
        });
    }
};
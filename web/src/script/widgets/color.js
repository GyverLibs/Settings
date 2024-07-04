import { Component } from "@alexgyver/component";
import WidgetBase from "./widget";
import { intToColor } from "./misc";

export default class ColorWidget extends WidgetBase {
    constructor(data) {
        super(data);

        super.addOutput(Component.make('div', {
            context: this,
            class: 'input_cont',
            children: [
                {
                    tag: 'input',
                    type: 'color',
                    class: 'color',
                    var: 'col',
                    attrs: { 'colorpick-eyedropper-active': false },
                    events: {
                        change: () => {
                            this.$out.style.background = this.$col.value;
                            let hex = this.$col.value.slice(1);
                            if (hex.length == 3) hex = hex.split('').map(x => x + x).join('');
                            this.sendEvent(parseInt(hex, 16));
                        }
                    }
                },
                {
                    tag: 'div',
                    class: 'color_out',
                    var: 'out',
                    events: {
                        click: () => this.$col.showPicker(),
                    }
                }
            ]
        }));

        this.update(data.value);
    }

    update(value) {
        this.$col.value = intToColor(value ?? 0);
        this.$out.style.background = this.$col.value;
    }
}
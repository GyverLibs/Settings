import { Component } from "@alexgyver/component";
import Page from "../page";
import { Arrow } from "./misc";
import WidgetBase from "./widget";

export default function MenuWidget(title, data, parent, pages, widgets) {
    let page = Page(data, pages, widgets);
    let widget = new WidgetBase({ label: title });
    widget.addOutput(Arrow('right', 20));

    Component.config(widget.$root, {
        style: 'cursor:pointer',
        events: {
            click: () => {
                page.dispatchEvent(new CustomEvent("menuclick", {
                    bubbles: true,
                    detail: {
                        page: page,
                        parent: parent,
                        label: title
                    }
                }));
            }
        }
    });
    return widget.$root;
};
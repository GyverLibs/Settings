import { Component } from "@alexgyver/component";
import Widget from "./widget";
import Page from "../page";
import { Arrow } from "./misc";

export default function MenuWidget(title, data, parent, pages, widgets) {
    let page = Page(data, pages, widgets);
    let widget = Widget({ label: title }, Arrow('right', 20));

    Component.config(widget, {
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
    return widget;
};
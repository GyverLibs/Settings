import { Component } from "@alexgyver/component";
import './dialog.css';

export class DialogCont {
    constructor() {
        Component.make('div', {
            context: this,
            var: 'root',
            class: 'dialog_back',
            style: 'animation: fadeIn 0.16s;',
            parent: document.body,
        });
    }

    destroy() {
        this.$root.style.animation = 'fadeOut 0.16s';
        setTimeout(() => this.$root.remove(), 150);
    }
}

export function BaseDialog(label, content, actionOK, actionCancel, postRender = null) {
    let dialog = new DialogCont();

    Component.config(dialog.$root, {
        child: {
            tag: 'div',
            class: 'dialog_cont',
            child: {
                tag: 'div',
                class: 'dialog',
                children: [
                    {
                        tag: 'label',
                        text: label,
                    },
                    content,
                    {
                        tag: 'div',
                        class: 'dialog_btns',
                        children: [
                            {
                                tag: 'div',
                                class: 'button',
                                text: 'OK',
                                events: {
                                    click: () => {
                                        actionOK();
                                        dialog.destroy();
                                    },
                                },
                            },
                            {
                                tag: 'div',
                                style: 'width: 20px',
                            },
                            {
                                tag: 'div',
                                class: 'button',
                                style: 'background: var(--error)',
                                text: 'Cancel',
                                events: {
                                    click: () => {
                                        actionCancel();
                                        dialog.destroy();
                                    },
                                },
                            }
                        ]
                    }
                ]
            }
        }
    });
    if (postRender) postRender();
}

export function AsyncPrompt(label, value) {
    return new Promise(resolve => {
        let area = Component.make('textarea', {
            text: value,
            rows: 1,
            events: {
                input: () => area.style.height = area.scrollHeight + "px",
            }
        });

        BaseDialog(label, area, () => resolve(area.value), () => resolve(null), () => {
            area.focus();
            area.setSelectionRange(area.value.length, area.value.length);  // cursor end
            area.style.height = area.scrollHeight + "px";
        });
    });
}

export function AsyncConfirm(label) {
    return new Promise(resolve => {
        BaseDialog(label, null, () => resolve(1), () => resolve(0));
    });
}
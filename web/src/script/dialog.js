import { Component } from "@alexgyver/component";

export function BaseDialog(label, content, actionOK, actionCancel) {
    let ctx = {};
    Component.make('div', {
        context: ctx,
        class: 'dialog_back',
        var: 'back',
        parent: document.body,
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
                                        ctx.$back.remove();
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
                                        ctx.$back.remove();
                                    },
                                },
                            }
                        ]
                    }
                ]
            }
        }
    });
}

export function AsyncPrompt(label, value) {
    return new Promise(resolve => {
        let area = Component.make('textarea', {
            text: value,
            events: {
                input: () => area.style.height = area.scrollHeight + "px",
            }
        });
        area.style.height = area.scrollHeight + "px";
        BaseDialog(label, area, () => resolve(area.value), () => resolve(null));
    });
}

export function AsyncConfirm(label) {
    return new Promise(resolve => {
        BaseDialog(label, null, () => resolve(1), () => resolve(0));
    });
}
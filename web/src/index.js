import './index.css'
import Settings from './script/settings';

let settings;

document.addEventListener("DOMContentLoaded", () => {
    settings = new Settings();
    // settings.render(JSON.stringify(
    //     {
    //         title: 'Menu!', content: [
    //             { type: 'input', id: 'sw1', label: 'text', value: 'abc' },
    //             { type: 'pass', id: 'sw1', label: 'pass', value: 'def' },
    //             {
    //                 type: 'group', title: '', content: [
    //                     { type: 'select', id: 'sw2', label: 'select', text: 'kek;puk;azaz', value: 1 },
    //                     { type: 'switch', id: 'sw3', label: 'switch', value: 1 },
    //                 ]
    //             },
    //             {
    //                 type: 'buttons', content: [
    //                     { id: 'b1', label: 'btn 1', color: 'aa0000' },
    //                     { id: 'b2', label: 'btn 2' },
    //                 ]
    //             },
    //             {
    //                 type: 'group', title: 'widgets', content: [
    //                     { type: 'date', id: 'sw2', label: 'date', value: 1719787851 },
    //                     { type: 'time', id: 'sw3', label: 'time', value: 1719787851 },
    //                     { type: 'datetime', id: 'sw3', label: 'datime', value: 1719787851 },
    //                     { type: 'slider', id: 'sw3', label: 'slider', value: 10, unit: '%' },
    //                     { type: 'label', id: 'sw3', label: 'label', value: 10 },
    //                     { type: 'color', id: 'sw3', label: 'color', value: 0xff0000 },
    //                     {
    //                         type: 'buttons', content: [
    //                             { id: 'b1', label: 'btn 1' },
    //                             { id: 'b2', label: 'btn 2' },
    //                             { id: 'b2', label: 'btn 2' },
    //                             { id: 'b2', label: 'btn 2' },
    //                         ]
    //                     },
    //                     { type: 'color', id: 'sw3', label: 'color', value: 0xff0000 },
    //                 ]
    //             },
    //             {
    //                 type: 'menu', label: 'Some menu', content: [
    //                     { type: 'switch', id: 'sw4', label: 'only switch 2', value: 1 },
    //                     { type: 'switch', id: 'sw4', label: 'only switch 2', value: 1 },
    //                     { type: 'switch', id: 'sw4', label: 'only switch 2', value: 1 },
    //                     {
    //                         type: 'group', title: 'some group', content: [
    //                             { type: 'switch', id: 'sw5', label: 'switch widget 3', value: 0 },
    //                             { type: 'switch', id: 'sw6', label: 'switch widget 4', value: 1 },
    //                             {
    //                                 type: 'menu', label: 'Some menu agan', content: [
    //                                     {
    //                                         type: 'group', title: '', content: [
    //                                             { type: 'switch', id: 'sw7', label: 'switch widget 5', value: 0 },
    //                                             { type: 'switch', id: 'sw8', label: 'switch widget 6', value: 1 },
    //                                         ]
    //                                     },
    //                                 ]
    //                             },
    //                         ]
    //                     },
    //                 ]
    //             }
    //         ]
    //     }
    // ));
});
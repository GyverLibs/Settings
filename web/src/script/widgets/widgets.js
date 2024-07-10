import ColorWidget from "./color";
import LabelWidget from "./label";
import PassWidget from "./pass";
import SelectWidget from "./select";
import SliderWidget from "./slider";
import SwitchWidget from "./switch";
import { DateWidget, DateTimeWidget, TimeWidget } from "./time";
import InputWidget from "./input";
import ButtonsWidget from "./buttons";
import ParagraphWidget from "./paragraph";
import Button from "./button";

export const WidgetList = {
    toggle: SwitchWidget,
    input: InputWidget,
    pass: PassWidget,
    select: SelectWidget,
    time: TimeWidget,
    date: DateWidget,
    datetime: DateTimeWidget,
    slider: SliderWidget,
    label: LabelWidget,
    color: ColorWidget,
    buttons: ButtonsWidget,
    button: Button,
    paragraph: ParagraphWidget,
}
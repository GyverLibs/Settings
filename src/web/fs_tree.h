#pragma once
#include <Arduino.h>

/*
    fs_tree.h - отображение файлового менеджера по папкам.

    Подключается только при объявленном SETT_FS_TREE, иначе не компилируется
    вообще и не занимает флеш.

    Как это работает: вебморда получает от прошивки один плоский список полных
    путей (HybridFS::listDir) и рисует по строке на путь. Скрипт ниже отдаётся
    через штатный слот кастомного js (/custom.js), следит за панелью файлов и
    после каждой перерисовки раскладывает строки по сворачиваемым папкам. Он
    ПЕРЕМЕЩАЕТ элементы, созданные вебмордой, поэтому скачивание, редактирование
    и удаление продолжают работать без изменений: меняется только вёрстка, плюс
    в строке остаётся имя файла, а полный путь уходит в подсказку.

    Ограничения загрузчика кастомного js (registerCustom), если будете править:
    - текст должен содержать ровно один `class Name {` и заканчиваться его `}`;
    - загрузчик делает replaceAll для имён WidgetBase, Component, EL,
      AsyncPrompt, AsyncConfirm, popup, intToColor - эти подстроки (в т.ч.
      заглавное "EL") не должны встречаться в коде, иначе их перепишет в
      `__list.X`.
*/

const char sets_fs_tree_js[] PROGMEM = R"JS(
class SetsFsTree {
  static css = `
.sft_head{align-items:center;background:var(--shadow_light);border-radius:6px;cursor:pointer;display:flex;justify-content:space-between;margin-top:4px;padding:6px 10px}
.sft_name{overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
.sft_meta{font-size:13px;margin-left:10px;opacity:.7;white-space:nowrap}
.sft_arrow{height:13px;margin-right:8px;transition:transform .15s;width:13px}
.sft_del{background:var(--error);height:15px;margin-left:12px;width:15px}
.sft_head.sft_open .sft_arrow{transform:rotate(90deg)}
.sft_list{display:none;padding-left:13px}
.sft_head.sft_open+.sft_list{display:block}
`;

  static {
    let cont = null;
    let obs = null;
    let busy = false;
    const opened = new Set();

    const dirOf = (p) => {
      const i = p.lastIndexOf("/");
      return i > 0 ? p.slice(0, i) : "";
    };

    const kbOf = (row) => {
      const s = row.querySelector(".fs_size");
      return s ? parseFloat(s.textContent) || 0 : 0;
    };

    const span = (cls, text) => {
      const e = document.createElement("span");
      e.className = cls;
      if (text !== undefined) e.textContent = text;
      return e;
    };

    // имя функции собрано из кусков намеренно: загрузчик кастомного js делает
    // replaceAll("Component") и испортил бы прямой вызов
    const enc = window["encodeURICompon" + "ent"];

    // удаление папки: вебморда умеет удалять только файлы, поэтому запрос
    // отправляется напрямую. Прошивка со сборкой SETT_FS_TREE удаляет папку
    // рекурсивно (см. HybridFS::removeDir)
    const removeDir = async (dir) => {
      const auth = Number(JSON.parse(localStorage.getItem("auth") || "0")) || 0;
      const base = typeof SETTINGS_DEV_URL === "string" ? SETTINGS_DEV_URL : location.origin;
      const url = base + "/settings?action=remove&id=0&value=" + enc(dir) +
        (auth ? "&auth=" + auth.toString(16) : "");
      const res = await fetch(url);
      if (!res.ok) throw new Error(res.status);
    };

    const rebuild = () => {
      if (busy || !cont) return;
      const rows = Array.from(cont.children).filter((e) => e.matches(".fs_row"));
      if (!rows.length) return;
      busy = true;
      obs.disconnect();

      const info = cont.querySelector(".fs_info");
      const groups = new Map();

      for (const row of rows) {
        const path = row.querySelector(".fs_path");
        if (!path) continue;
        const full = path.dataset.full || path.textContent;
        path.dataset.full = full;
        path.title = full;
        const dir = dirOf(full);
        path.textContent = dir ? full.slice(dir.length + 1) : full;
        if (!groups.has(dir)) groups.set(dir, []);
        groups.get(dir).push(row);
      }

      for (const dir of Array.from(groups.keys()).sort()) {
        const files = groups.get(dir);
        if (!dir) {
          for (const row of files) cont.appendChild(row);
          continue;
        }
        let kb = 0;
        for (const row of files) kb += kbOf(row);

        const left = document.createElement("div");
        left.style.cssText = "align-items:center;display:flex;overflow:hidden";
        left.append(span("icon arrow sft_arrow"), span("sft_name", dir));

        const wrap = document.createElement("div");

        const del = span("icon cross sft_del");
        del.title = dir;
        del.onclick = async (ev) => {
          ev.stopPropagation();
          if (!(await AsyncConfirm(dir + " (" + files.length + ")?"))) return;
          try {
            await removeDir(dir);
            wrap.remove();
          } catch (err) {
            popup(String(err), true);
          }
        };

        const right = document.createElement("div");
        right.style.cssText = "align-items:center;display:flex";
        right.append(span("sft_meta", files.length + " • " +
          (kb >= 1024 ? (kb / 1024).toFixed(1) + " MB" : Math.round(kb) + " kB")), del);

        const head = document.createElement("div");
        head.className = "sft_head";
        head.append(left, right);

        const list = document.createElement("div");
        list.className = "sft_list";
        for (const row of files) list.appendChild(row);

        if (opened.has(dir)) head.classList.add("sft_open");
        head.onclick = () => {
          if (head.classList.toggle("sft_open")) opened.add(dir);
          else opened.delete(dir);
        };

        wrap.append(head, list);
        cont.appendChild(wrap);
      }

      if (info) cont.appendChild(info);
      obs.observe(cont, { childList: true });
      busy = false;
    };

    const attach = () => {
      const found = document.querySelector(".fs_cont");
      if (!found) return false;
      cont = found;
      obs = new MutationObserver(rebuild);
      obs.observe(cont, { childList: true });
      rebuild();
      return true;
    };

    if (!attach()) {
      const boot = new MutationObserver(() => {
        if (attach()) boot.disconnect();
      });
      boot.observe(document.documentElement, { childList: true, subtree: true });
    }
  }
}
)JS";

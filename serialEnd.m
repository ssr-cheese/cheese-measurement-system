function serialEnd
global esp32_serial;

%serial終了処理追加
if ~isempty(esp32_serial)
    fclose(esp32_serial);
    delete(esp32_serial);
end

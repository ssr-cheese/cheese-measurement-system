function serialEnd
global esp32_serial;

%serial�I�������ǉ�
if ~isempty(esp32_serial)
    fclose(esp32_serial);
    delete(esp32_serial);
end

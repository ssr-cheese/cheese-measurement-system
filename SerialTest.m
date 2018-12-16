clear;
close all;

serial_list = seriallist;
%serial_list = ["COM1","COM6"];


%COM�͊��ɍ��킹�ĕς���K�v���肻��
if ~isempty( find(serial_list == "COM4") )
    esp32_serial = serial('COM4','BaudRate',115200);
    esp32_serial.Timeout = 0.01;   %10m�b�Ń^�C���A�E�g
    fopen(esp32_serial);
   
    %�f���̂��߂�200 * 0.1 = 20�b���[�v
    for now_time = 1:200  
        [strings,count,msg] = fgets(esp32_serial);
        if isempty(msg)
            [time,sensor_id] = parseForCheese(strings,"@",5,2)
        end
        
        pause(0.1);
    end
    fclose(esp32_serial);
    delete(esp32_serial);
end

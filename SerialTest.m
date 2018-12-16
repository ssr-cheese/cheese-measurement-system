clear;
close all;

serial_list = seriallist;
%serial_list = ["COM1","COM6"];


%COMは環境に合わせて変える必要ありそう
if ~isempty( find(serial_list == "COM4") )
    esp32_serial = serial('COM4','BaudRate',115200);
    esp32_serial.Timeout = 0.01;   %10m秒でタイムアウト
    fopen(esp32_serial);
   
    %デモのために200 * 0.1 = 20秒ループ
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

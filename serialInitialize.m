function serialInitialize(com_name,baudrate,app)
global esp32_serial
%serialObject������
%COM�͊��ɍ��킹�ĕς���K�v���肻��
%timer����ɃV���A�����������Ȃ��ƁCtimerHandle���N�����ď������O�ɃV���A���ǂݍ��݂����Ⴄ
serial_list = seriallist;
if ~isempty( find(serial_list == com_name,1) )
    esp32_serial = serial(com_name,'BaudRate',baudrate);
    esp32_serial.BytesAvailableFcn = @(src,event)serialCallback2(src,event,app);
    fopen(esp32_serial);
else
    serial_result = "serial failed" %serial���s�������́C�Ƃ肠�������s��\�������͂��Ƃ�
    %close;
end

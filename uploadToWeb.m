function uploadToWeb(api_accessor, mouse_index)

robot_data = load("robot" + num2str(mouse_index - 1) + ".mat");

running_log = robot_data.individualData;

num_of_depature = size(running_log, 1);

%send running_log to spreadsheet
for i = 1:num_of_depature
    
    if running_log{i,2} == 'R'
        api_accessor.updateTime(mouse_index , i, running_log{i,2});
    else
        %convert time(string) to time(double)
        api_accessor.updateTime(mouse_index, i, inverseFormatTime( running_log{i,2} ) );
    end
end
clear;

api_accessor = ApiAccessor;

runners_num = 1;

for mouse_index = 1:runners_num
    
    %get player data
    [mouse_name, player_name, org_name] = api_accessor.getDetail(mouse_index);
    
    app = mousetimer(mouse_index,mouse_name, player_name, org_name);
    
    %serialInitialize('COM5',115200,app);
    
    pause;  %wait for one player complete. you must manualy press any key if one player complete.
    
    % If app is closed...
    robot_data = load("robot" + num2str(mouse_index) + ".mat");
    
    running_log = robot_data.individualData;
    
    num_of_depature = size(running_log, 1);
    
    %send running_log to spreadsheet
    for i = 1:num_of_depature
        api_accessor.updateTime(mouse_index, i, running_log{i,2});
    end
    
end

% Push "Begin" button on the app, and hit any key on the command window
pause;
pause;

%serialEnd;


%app.saveData
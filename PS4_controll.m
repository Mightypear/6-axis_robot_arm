try delete(Arduino); end
clear
close all
clc
%% Setup
serialportlist
%% Kommentarad Arduino-inställning
port = "COM8"; baudRate = 9600;
global Arduino
Arduino = serialport(port, baudRate, "Timeout",5)
Arduino.configureTerminator('LF');
Arduino.configureCallback("terminator", @GetSerialData)

%% Globala variabler
global joystick buttonsState buttonsStatePrev tSend
global ax1
ax1 = 0;
joystick = vrjoystick(1);
t = timer('ExecutionMode','fixedRate','Period',0.01,'TimerFcn',@Controller_Callback);
start(t);
tSend = tic;
buttonsState = false(1,14);
buttonsStatePrev = buttonsState;

%% Funktion för att läsa seriell data från Arduino (om nödvändigt)
function GetSerialData(src, dataAvailableInfo)
    raw = fgets(src);
    disp(raw)
end

%% Huvudfunktion för att läsa av kontroller och hantera input
function Controller_Callback(src, evnt)
    global joystick buttonsState buttonsStatePrev Arduino tSend
    global ax1

    try
        [ax, btns, povs] = read(joystick); % Läs av joysticken
    catch
        joystick = vrjoystick(1); % Om det blir fel, starta om joysticken
        return;
    end

    povs; %ger pilknapparnas värden i grader 0-360
%upp = 0
%höger = 90
%ner = 180
%vänster = 270
%standbyvärde = -1




    btns; %ger knapparnas värden 0 eller 1, krashar gärna matlab 
    % key-bindnings:
square = btns(1);
X = btns(2);
O = btns(3);
triangel = btns(4);
L1 = btns(5);
R1 = btns(6);
L2 = btns(7);
R2 = btns(8);
share = btns(9);
options = btns(10);
L3 = btns(11);
R3 = btns(12);
PS = btns(13);
touchpad = btns(14);

%keys = [square, X, O, triangel, L1, R1, L2, R2, share, options, L3, R3, PS, touchpad]
% visar samma som btns, bara lättare att förstå, krashar gärna matlab




    % Läs av vänster joystick (axel 1 och 2)
    leftStickX = ax(1);
    leftStickY = ax(2);

    % Läs av höger joystick (axel 3 och 6)
    rightStickX = ax(3);
    rightStickY = ax(6); %tydligen är axel 6 Y-axeln på högra spaken

    % Hantera axel 3 för att styra höger joystick
    if btns(5) == 1  % Lagt till säkring igen(L1), testa om det fungerar 
        if btns(13) == 1 && buttonsStatePrev(13) == 0
            if (buttonsState(13) == 1)
                buttonsState(13) = 0;
                disp('State on')
                disp("EnableAll") % PS-knappen
                writeline(Arduino, "EnableAll")
            else
                buttonsState(13) = 1;
                disp('State off')
                disp("DisableAll") % PS-knappen
                writeline(Arduino, "DisableAll")
            end
        end % Lagt till säkring igen(L1), testa om det fungerar 
        buttonsStatePrev = btns;
    end
    
% GET POSITION %
%if btns(11) == 1 && buttonsStatePrev(11) == 0
    if btns(14) == 1 && buttonsStatePrev(14) == 0
            %disp('A pressed')
            %disp("GetPosition") % Touchpad
            writeline(Arduino, "GetAllPositions")
            %pause 0.001
    end 
%end
% SET HOME %
    if btns(9) == 1 && buttonsStatePrev(9) == 0
            %disp('A pressed')
            disp("Is this home?") % Share-knappen
            writeline(Arduino, "SetHome")
    end
    
% GO HOME %
    if btns(10) == 1 && buttonsStatePrev(10) == 0
            %disp('A pressed')
            disp("Going home now") % Options-knappen
            writeline(Arduino, "GoHome")
    end
% SET P1

        % if btns(4) == 1 && buttonsStatePrev(4) == 0
        %     if (buttonsState(4) == 1)
        %         buttonsState(4) = 0;
        %         %disp('State on')
        %         disp("Recordin P1") % PS-knappen
        %         writeline(Arduino, "SetP1")
        %     else
        %         buttonsState(4) = 1;
        %         %disp('State off')
        %         disp("Returning to P1") % PS-knappen
        %         writeline(Arduino, "GoP1")
        %     end
        % end



% SET P1, 2, 3, 4 %
if btns(11) == 1
 if btns(1) == 1 && buttonsStatePrev(1) == 0
            %disp('A pressed')
            disp("Recording P1") % Fyrkant
            writeline(Arduino, "SetP1")
 end
  if btns(2) == 1 && buttonsStatePrev(2) == 0
            %disp('A pressed')
            disp("Recording P2") % X
            writeline(Arduino, "SetP2")
  end
   if btns(3) == 1 && buttonsStatePrev(3) == 0
            %disp('A pressed')
            disp("Recording P3") % Cirkel
            writeline(Arduino, "SetP3")
   end
   if btns(4) == 1 && buttonsStatePrev(4) == 0
            %disp('A pressed')
            disp("Recording P4") % Triangel
            writeline(Arduino, "SetP4")
 end
end
% GO P1, 2, 3, 4 %

    if btns(1) == 1 && buttonsStatePrev(1) == 0
            %disp('A pressed')
            disp("Going to P1") % L2 + triangel
            writeline(Arduino, "GoP1")
    end
    if btns(2) == 1 && buttonsStatePrev(2) == 0
            %disp('A pressed')
            disp("Going to P2") % L2 + triangel
            writeline(Arduino, "GoP2")
    end
    if btns(3) == 1 && buttonsStatePrev(3) == 0
            %disp('A pressed')
            disp("Going to P3") % L2 + triangel
            writeline(Arduino, "GoP3")
    end
    if btns(4) == 1 && buttonsStatePrev(4) == 0
            %disp('A pressed')
            disp("Going to P4") % L2 + triangel
            writeline(Arduino, "GoP4")
    end
  
    

% %GRIPPER
% 
% 
%    if btns(3) == 1 && buttonsStatePrev(3) == 0
%             disp('Gripping')
%             disp("Gripping") % X
%             writeline(Arduino, "GoServo 5")
%         end
%         buttonsStatePrev = btns;
%    if btns(2) == 1 && buttonsStatePrev(2) == 0
%             disp('Un-Gripping')
%             disp("Un-Gripping") % O
%             writeline(Arduino, "GoServo -5")
%    end
% 
%         buttonsStatePrev = btns;
% 
% 
% %GRIPPER


    % % Vänster joystick rörelse
    if btns(5) == 1
        %if abs(leftStickX) > 0.2
        if (toc(tSend) > 0.2) % Skicka var 0.2 sekunder
            tSend = tic();
           
            lx = sprintf("GoRel 0 %0.4f", leftStickX * -2); % Rotera platform
            ly = sprintf("GoRel 2 %0.3f", leftStickY * 2); % Höj och sänk stickan
       
            
             writeline(Arduino, lx) % VIKTIGT SÄTT TILLBAKA!!
             writeline(Arduino, ly) % VIKTIGT SÄTT TILLBAKA!!



          
        end
    end

    
     if btns(6) == 1
     %if abs(rightStickX) > 0.2
        if (toc(tSend) > 0.2) % Skicka var 0.2 sekunder
            tSend = tic();
            %rx = sprintf("HögerX 0 %0.3f", rightStickX * 5);
            %ry = sprintf("HögerY 0 %0.3f", rightStickY * -5); %-5 för positiv riktning


             %DESSA TVÅ FUNGERAR, rx beter sig konstigt, kan vara fel pins
            rx = sprintf("GoRel 4 %0.3f", rightStickX * 1); % Vik in
            ry = sprintf("GoRel 1 %0.3f", rightStickY * 3);% Höj och sänk bom



            %r = sprintf("GoRel 0 %0.3f", rightstickX = 5); 
           
            %disp(rx)
            %disp(ry)
            


            writeline(Arduino, rx) % VIKTIGT SÄTT TILLBAKA!!
            writeline(Arduino, ry) % VIKTIGT SÄTT TILLBAKA!!
      
        end

     end
% ANDRA FUNK R
     if btns(8) == 1
     %if abs(rightStickX) > 0.2
        if (toc(tSend) > 0.2) % Skicka var 0.2 sekunder
            tSend = tic();
            %rx = sprintf("HögerX 0 %0.3f", rightStickX * 5);
            %ry = sprintf("HögerY 0 %0.3f", rightStickY * -5); %-5 för positiv riktning


             %DESSA TVÅ FUNGERAR, rx beter sig konstigt, kan vara fel pins
            rx = sprintf("GoRel 3 %0.3f", rightStickX * 1); % Vik in
            ry = sprintf("GoRel 5 %0.3f", rightStickY * 3);% Höj och sänk bom


            %r = sprintf("GoRel 0 %0.3f", rightstickX = 5); 
          
            %disp(rx);
            %disp(ry);
            
            writeline(Arduino, rx) % VIKTIGT SÄTT TILLBAKA!!
            writeline(Arduino, ry) % VIKTIGT SÄTT TILLBAKA!!

        end

     end

     %ANDRA FUNK R


    % % FELSÖKNING, kör för att se individuella "axlar", gör eventuellt kaos
    % if abs(leftStickX) > 0.2
    %     disp(['Vänster joystick (X): ', num2str(leftStickX)]);
    % end
    % if abs(leftStickY) > 0.2
    %     disp(['Vänster joystick (Y): ', num2str(leftStickY)]);
    % end
    % if abs(rightStickX) > 0.2
    %     disp(['Höger joystick (X): ', num2str(rightStickX)]);
    % end
    % if abs(rightStickY) > 0.2
    %     disp(['Höger joystick (Y): ', num2str(rightStickY)]);
    %  end
end

    function targMap = targDataMap(),

    ;%***********************
    ;% Create Parameter Map *
    ;%***********************
    
        nTotData      = 0; %add to this count as we go
        nTotSects     = 1;
        sectIdxOffset = 0;

        ;%
        ;% Define dummy sections & preallocate arrays
        ;%
        dumSection.nData = -1;
        dumSection.data  = [];

        dumData.logicalSrcIdx = -1;
        dumData.dtTransOffset = -1;

        ;%
        ;% Init/prealloc paramMap
        ;%
        paramMap.nSections           = nTotSects;
        paramMap.sectIdxOffset       = sectIdxOffset;
            paramMap.sections(nTotSects) = dumSection; %prealloc
        paramMap.nTotData            = -1;

        ;%
        ;% Auto data (rtP)
        ;%
            section.nData     = 38;
            section.data(38)  = dumData; %prealloc

                    ;% rtP.DC_Amplitude
                    section.data(1).logicalSrcIdx = 0;
                    section.data(1).dtTransOffset = 0;

                    ;% rtP.PID_Controller_e_1_I
                    section.data(2).logicalSrcIdx = 1;
                    section.data(2).dtTransOffset = 1;

                    ;% rtP.PID_Controller_e_2_I
                    section.data(3).logicalSrcIdx = 2;
                    section.data(3).dtTransOffset = 2;

                    ;% rtP.PID_Controller_i_1_I
                    section.data(4).logicalSrcIdx = 3;
                    section.data(4).dtTransOffset = 3;

                    ;% rtP.PID_Controller_i_2_I
                    section.data(5).logicalSrcIdx = 4;
                    section.data(5).dtTransOffset = 4;

                    ;% rtP.PID_Controller_e_1_InitialConditionForIntegrator
                    section.data(6).logicalSrcIdx = 5;
                    section.data(6).dtTransOffset = 5;

                    ;% rtP.PID_Controller_i_1_InitialConditionForIntegrator
                    section.data(7).logicalSrcIdx = 6;
                    section.data(7).dtTransOffset = 6;

                    ;% rtP.PID_Controller_e_2_InitialConditionForIntegrator
                    section.data(8).logicalSrcIdx = 7;
                    section.data(8).dtTransOffset = 7;

                    ;% rtP.PID_Controller_i_2_InitialConditionForIntegrator
                    section.data(9).logicalSrcIdx = 8;
                    section.data(9).dtTransOffset = 8;

                    ;% rtP.PID_Controller_e_1_P
                    section.data(10).logicalSrcIdx = 9;
                    section.data(10).dtTransOffset = 9;

                    ;% rtP.PID_Controller_i_1_P
                    section.data(11).logicalSrcIdx = 10;
                    section.data(11).dtTransOffset = 10;

                    ;% rtP.PID_Controller_e_2_P
                    section.data(12).logicalSrcIdx = 11;
                    section.data(12).dtTransOffset = 11;

                    ;% rtP.PID_Controller_i_2_P
                    section.data(13).logicalSrcIdx = 12;
                    section.data(13).dtTransOffset = 12;

                    ;% rtP.StateSpace_AS_param
                    section.data(14).logicalSrcIdx = 13;
                    section.data(14).dtTransOffset = 13;

                    ;% rtP.StateSpace_BS_param
                    section.data(15).logicalSrcIdx = 14;
                    section.data(15).dtTransOffset = 29;

                    ;% rtP.StateSpace_CS_param
                    section.data(16).logicalSrcIdx = 15;
                    section.data(16).dtTransOffset = 65;

                    ;% rtP.StateSpace_DS_param
                    section.data(17).logicalSrcIdx = 16;
                    section.data(17).dtTransOffset = 109;

                    ;% rtP.StateSpace_X0_param
                    section.data(18).logicalSrcIdx = 17;
                    section.data(18).dtTransOffset = 208;

                    ;% rtP.donotdeletethisgain_Gain
                    section.data(19).logicalSrcIdx = 18;
                    section.data(19).dtTransOffset = 212;

                    ;% rtP.trigger_Value
                    section.data(20).logicalSrcIdx = 19;
                    section.data(20).dtTransOffset = 213;

                    ;% rtP.Switch_Threshold
                    section.data(21).logicalSrcIdx = 20;
                    section.data(21).dtTransOffset = 214;

                    ;% rtP.donotdeletethisgain_Gain_czz01xtb3a
                    section.data(22).logicalSrcIdx = 21;
                    section.data(22).dtTransOffset = 215;

                    ;% rtP.Constant3_Value
                    section.data(23).logicalSrcIdx = 22;
                    section.data(23).dtTransOffset = 216;

                    ;% rtP.Constant2_Value
                    section.data(24).logicalSrcIdx = 23;
                    section.data(24).dtTransOffset = 217;

                    ;% rtP.uib1_Gain
                    section.data(25).logicalSrcIdx = 24;
                    section.data(25).dtTransOffset = 218;

                    ;% rtP.Gain_Gain
                    section.data(26).logicalSrcIdx = 25;
                    section.data(26).dtTransOffset = 219;

                    ;% rtP.Switch1_Threshold
                    section.data(27).logicalSrcIdx = 26;
                    section.data(27).dtTransOffset = 220;

                    ;% rtP.donotdeletethisgain_Gain_o0wxvljlan
                    section.data(28).logicalSrcIdx = 27;
                    section.data(28).dtTransOffset = 221;

                    ;% rtP.Constant3_Value_hw4zakmrfm
                    section.data(29).logicalSrcIdx = 28;
                    section.data(29).dtTransOffset = 222;

                    ;% rtP.Constant2_Value_lsteepdquw
                    section.data(30).logicalSrcIdx = 29;
                    section.data(30).dtTransOffset = 223;

                    ;% rtP.uib1_Gain_bsvwuaxwpi
                    section.data(31).logicalSrcIdx = 30;
                    section.data(31).dtTransOffset = 224;

                    ;% rtP.Gain1_Gain
                    section.data(32).logicalSrcIdx = 31;
                    section.data(32).dtTransOffset = 225;

                    ;% rtP.SwitchCurrents_Value
                    section.data(33).logicalSrcIdx = 32;
                    section.data(33).dtTransOffset = 226;

                    ;% rtP.Constant_Value
                    section.data(34).logicalSrcIdx = 33;
                    section.data(34).dtTransOffset = 234;

                    ;% rtP.Constant1_Value
                    section.data(35).logicalSrcIdx = 34;
                    section.data(35).dtTransOffset = 235;

                    ;% rtP.Constant2_Value_ahwqlmii2b
                    section.data(36).logicalSrcIdx = 35;
                    section.data(36).dtTransOffset = 236;

                    ;% rtP.Vref1_Value
                    section.data(37).logicalSrcIdx = 36;
                    section.data(37).dtTransOffset = 237;

                    ;% rtP.Vref2_Value
                    section.data(38).logicalSrcIdx = 37;
                    section.data(38).dtTransOffset = 238;

            nTotData = nTotData + section.nData;
            paramMap.sections(1) = section;
            clear section


            ;%
            ;% Non-auto Data (parameter)
            ;%


        ;%
        ;% Add final counts to struct.
        ;%
        paramMap.nTotData = nTotData;



    ;%**************************
    ;% Create Block Output Map *
    ;%**************************
    
        nTotData      = 0; %add to this count as we go
        nTotSects     = 1;
        sectIdxOffset = 0;

        ;%
        ;% Define dummy sections & preallocate arrays
        ;%
        dumSection.nData = -1;
        dumSection.data  = [];

        dumData.logicalSrcIdx = -1;
        dumData.dtTransOffset = -1;

        ;%
        ;% Init/prealloc sigMap
        ;%
        sigMap.nSections           = nTotSects;
        sigMap.sectIdxOffset       = sectIdxOffset;
            sigMap.sections(nTotSects) = dumSection; %prealloc
        sigMap.nTotData            = -1;

        ;%
        ;% Auto data (rtB)
        ;%
            section.nData     = 30;
            section.data(30)  = dumData; %prealloc

                    ;% rtB.osxfsl0cym
                    section.data(1).logicalSrcIdx = 0;
                    section.data(1).dtTransOffset = 0;

                    ;% rtB.k2dlruxx5u
                    section.data(2).logicalSrcIdx = 1;
                    section.data(2).dtTransOffset = 11;

                    ;% rtB.hsyb5sy2fc
                    section.data(3).logicalSrcIdx = 2;
                    section.data(3).dtTransOffset = 19;

                    ;% rtB.djugjdenza
                    section.data(4).logicalSrcIdx = 3;
                    section.data(4).dtTransOffset = 20;

                    ;% rtB.cgdfmnvyw5
                    section.data(5).logicalSrcIdx = 4;
                    section.data(5).dtTransOffset = 21;

                    ;% rtB.lcqe1riv5e
                    section.data(6).logicalSrcIdx = 5;
                    section.data(6).dtTransOffset = 22;

                    ;% rtB.fnw34rqsrd
                    section.data(7).logicalSrcIdx = 6;
                    section.data(7).dtTransOffset = 23;

                    ;% rtB.ggnovrpbca
                    section.data(8).logicalSrcIdx = 7;
                    section.data(8).dtTransOffset = 24;

                    ;% rtB.ln23cmyrrk
                    section.data(9).logicalSrcIdx = 8;
                    section.data(9).dtTransOffset = 25;

                    ;% rtB.if2ke03cmg
                    section.data(10).logicalSrcIdx = 9;
                    section.data(10).dtTransOffset = 26;

                    ;% rtB.osvfaydsul
                    section.data(11).logicalSrcIdx = 10;
                    section.data(11).dtTransOffset = 27;

                    ;% rtB.gfgi4gjdtc
                    section.data(12).logicalSrcIdx = 11;
                    section.data(12).dtTransOffset = 28;

                    ;% rtB.hvhda0al5i
                    section.data(13).logicalSrcIdx = 12;
                    section.data(13).dtTransOffset = 29;

                    ;% rtB.bdj2lr3e5n
                    section.data(14).logicalSrcIdx = 13;
                    section.data(14).dtTransOffset = 30;

                    ;% rtB.howhprf3lv
                    section.data(15).logicalSrcIdx = 14;
                    section.data(15).dtTransOffset = 31;

                    ;% rtB.aoadtzkehg
                    section.data(16).logicalSrcIdx = 15;
                    section.data(16).dtTransOffset = 32;

                    ;% rtB.ejfa4qebmu
                    section.data(17).logicalSrcIdx = 16;
                    section.data(17).dtTransOffset = 33;

                    ;% rtB.jjy21yotzy
                    section.data(18).logicalSrcIdx = 17;
                    section.data(18).dtTransOffset = 34;

                    ;% rtB.gmw5rkyhks
                    section.data(19).logicalSrcIdx = 18;
                    section.data(19).dtTransOffset = 35;

                    ;% rtB.hqifyhyn5g
                    section.data(20).logicalSrcIdx = 19;
                    section.data(20).dtTransOffset = 36;

                    ;% rtB.ofeouq3efa
                    section.data(21).logicalSrcIdx = 20;
                    section.data(21).dtTransOffset = 37;

                    ;% rtB.c0fpagk2j4
                    section.data(22).logicalSrcIdx = 21;
                    section.data(22).dtTransOffset = 38;

                    ;% rtB.ha3c1iodke
                    section.data(23).logicalSrcIdx = 22;
                    section.data(23).dtTransOffset = 39;

                    ;% rtB.ehbg2lyldn
                    section.data(24).logicalSrcIdx = 23;
                    section.data(24).dtTransOffset = 40;

                    ;% rtB.i0iuxica0n
                    section.data(25).logicalSrcIdx = 24;
                    section.data(25).dtTransOffset = 41;

                    ;% rtB.bdsk5vrvux
                    section.data(26).logicalSrcIdx = 25;
                    section.data(26).dtTransOffset = 42;

                    ;% rtB.pclhdfxm1d
                    section.data(27).logicalSrcIdx = 26;
                    section.data(27).dtTransOffset = 43;

                    ;% rtB.gb2fvfaqkx
                    section.data(28).logicalSrcIdx = 27;
                    section.data(28).dtTransOffset = 44;

                    ;% rtB.g2ya2r5i0g
                    section.data(29).logicalSrcIdx = 28;
                    section.data(29).dtTransOffset = 45;

                    ;% rtB.gf4nlt3vfv
                    section.data(30).logicalSrcIdx = 29;
                    section.data(30).dtTransOffset = 46;

            nTotData = nTotData + section.nData;
            sigMap.sections(1) = section;
            clear section


            ;%
            ;% Non-auto Data (signal)
            ;%


        ;%
        ;% Add final counts to struct.
        ;%
        sigMap.nTotData = nTotData;



    ;%*******************
    ;% Create DWork Map *
    ;%*******************
    
        nTotData      = 0; %add to this count as we go
        nTotSects     = 4;
        sectIdxOffset = 1;

        ;%
        ;% Define dummy sections & preallocate arrays
        ;%
        dumSection.nData = -1;
        dumSection.data  = [];

        dumData.logicalSrcIdx = -1;
        dumData.dtTransOffset = -1;

        ;%
        ;% Init/prealloc dworkMap
        ;%
        dworkMap.nSections           = nTotSects;
        dworkMap.sectIdxOffset       = sectIdxOffset;
            dworkMap.sections(nTotSects) = dumSection; %prealloc
        dworkMap.nTotData            = -1;

        ;%
        ;% Auto data (rtDW)
        ;%
            section.nData     = 1;
            section.data(1)  = dumData; %prealloc

                    ;% rtDW.gwi4vt3wpl
                    section.data(1).logicalSrcIdx = 0;
                    section.data(1).dtTransOffset = 0;

            nTotData = nTotData + section.nData;
            dworkMap.sections(1) = section;
            clear section

            section.nData     = 7;
            section.data(7)  = dumData; %prealloc

                    ;% rtDW.oi02hmlpyi.AS
                    section.data(1).logicalSrcIdx = 1;
                    section.data(1).dtTransOffset = 0;

                    ;% rtDW.lhus1uexmq.LoggedData
                    section.data(2).logicalSrcIdx = 2;
                    section.data(2).dtTransOffset = 1;

                    ;% rtDW.fgy1lz20tg.LoggedData
                    section.data(3).logicalSrcIdx = 3;
                    section.data(3).dtTransOffset = 3;

                    ;% rtDW.gss0hbbl3l.LoggedData
                    section.data(4).logicalSrcIdx = 4;
                    section.data(4).dtTransOffset = 4;

                    ;% rtDW.dtiqwrir45.LoggedData
                    section.data(5).logicalSrcIdx = 5;
                    section.data(5).dtTransOffset = 5;

                    ;% rtDW.gdqhx2icer.LoggedData
                    section.data(6).logicalSrcIdx = 6;
                    section.data(6).dtTransOffset = 6;

                    ;% rtDW.mrhxip3nk1.AQHandles
                    section.data(7).logicalSrcIdx = 7;
                    section.data(7).dtTransOffset = 7;

            nTotData = nTotData + section.nData;
            dworkMap.sections(2) = section;
            clear section

            section.nData     = 1;
            section.data(1)  = dumData; %prealloc

                    ;% rtDW.ehenhsl3i5
                    section.data(1).logicalSrcIdx = 8;
                    section.data(1).dtTransOffset = 0;

            nTotData = nTotData + section.nData;
            dworkMap.sections(3) = section;
            clear section

            section.nData     = 2;
            section.data(2)  = dumData; %prealloc

                    ;% rtDW.dwdcdvhiev
                    section.data(1).logicalSrcIdx = 9;
                    section.data(1).dtTransOffset = 0;

                    ;% rtDW.p0c3lv2aj4
                    section.data(2).logicalSrcIdx = 10;
                    section.data(2).dtTransOffset = 1;

            nTotData = nTotData + section.nData;
            dworkMap.sections(4) = section;
            clear section


            ;%
            ;% Non-auto Data (dwork)
            ;%


        ;%
        ;% Add final counts to struct.
        ;%
        dworkMap.nTotData = nTotData;



    ;%
    ;% Add individual maps to base struct.
    ;%

    targMap.paramMap  = paramMap;
    targMap.signalMap = sigMap;
    targMap.dworkMap  = dworkMap;

    ;%
    ;% Add checksums to base struct.
    ;%


    targMap.checksum0 = 768363794;
    targMap.checksum1 = 1091937133;
    targMap.checksum2 = 483167757;
    targMap.checksum3 = 2605760807;


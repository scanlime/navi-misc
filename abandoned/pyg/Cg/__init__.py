""" Cg

This is a python package providing a wrapper to nVidia's Cg runtime
"""
#
# Python Cg Package
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
from Cg import *
from CgGL import *

# overwrite some functions, due to arrays and such
def cgCreateProgramFromFile(context, type, file, profile, entry, args):
    import Cg
    if args is None:
        return Cg.cgCreateProgramFromFile(context, type, file, profile, entry, args)
    else:
        pargs = Cg.new_stringArray(len(args))
        if len(args) > 0:
            for index,arg in args,range(len(args)):
                Cg.stringArray_setitem(pargs, index, arg)
        prog = Cg.cgCreateProgramFromFile(context, type, file, profile, entry, pargs)
        Cg.delete_stringArray(pargs)
	return prog

# yay for constants
CG_UNKNOWN_TYPE                   = 0
CG_STRUCT                         = 1
CG_ARRAY                          = 2
CG_TYPE_START_ENUM                = 1024
CG_HALF                           = 1025
CG_HALF2                          = 1026
CG_HALF3                          = 1027
CG_HALF4                          = 1028
CG_HALF1x1                        = 1029
CG_HALF1x2                        = 1030
CG_HALF1x3                        = 1031
CG_HALF1x4                        = 1032
CG_HALF2x1                        = 1033
CG_HALF2x2                        = 1034
CG_HALF2x3                        = 1035
CG_HALF2x4                        = 1036
CG_HALF3x1                        = 1037
CG_HALF3x2                        = 1038
CG_HALF3x3                        = 1039
CG_HALF3x4                        = 1040
CG_HALF4x1                        = 1041
CG_HALF4x2                        = 1042
CG_HALF4x3                        = 1043
CG_HALF4x4                        = 1044
CG_FLOAT                          = 1045
CG_FLOAT2                         = 1046
CG_FLOAT3                         = 1047
CG_FLOAT4                         = 1048
CG_FLOAT1x1                       = 1049
CG_FLOAT1x2                       = 1050
CG_FLOAT1x3                       = 1051
CG_FLOAT1x4                       = 1052
CG_FLOAT2x1                       = 1053
CG_FLOAT2x2                       = 1054
CG_FLOAT2x3                       = 1055
CG_FLOAT2x4                       = 1056
CG_FLOAT3x1                       = 1057
CG_FLOAT3x2                       = 1058
CG_FLOAT3x3                       = 1059
CG_FLOAT3x4                       = 1060
CG_FLOAT4x1                       = 1061
CG_FLOAT4x2                       = 1062
CG_FLOAT4x3                       = 1063
CG_FLOAT4x4                       = 1064
CG_SAMPLER1D                      = 1065
CG_SAMPLER2D                      = 1066
CG_SAMPLER3D                      = 1067
CG_SAMPLERRECT                    = 1068
CG_SAMPLERCUBE                    = 1069
CG_FIXED                          = 1070
CG_FIXED2                         = 1071
CG_FIXED3                         = 1072
CG_FIXED4                         = 1073
CG_FIXED1x1                       = 1074
CG_FIXED1x2                       = 1075
CG_FIXED1x3                       = 1076
CG_FIXED1x4                       = 1077
CG_FIXED2x1                       = 1078
CG_FIXED2x2                       = 1079
CG_FIXED2x3                       = 1080
CG_FIXED2x4                       = 1081
CG_FIXED3x1                       = 1082
CG_FIXED3x2                       = 1083
CG_FIXED3x3                       = 1084
CG_FIXED3x4                       = 1085
CG_FIXED4x1                       = 1086
CG_FIXED4x2                       = 1087
CG_FIXED4x3                       = 1088
CG_FIXED4x4                       = 1089
CG_HALF1                          = 1090
CG_FLOAT1                         = 1091
CG_FIXED1                         = 1092

CG_TEXUINT0                       = 2048
CG_TEXUINT1                       = 2049
CG_TEXUINT2                       = 2050
CG_TEXUINT3                       = 2051
CG_TEXUINT4                       = 2052
CG_TEXUINT5                       = 2053
CG_TEXUINT6                       = 2054
CG_TEXUINT7                       = 2055
CG_TEXUINT8                       = 2056
CG_TEXUINT9                       = 2057
CG_TEXUINT10                      = 2058
CG_TEXUINT11                      = 2059
CG_TEXUINT12                      = 2060
CG_TEXUINT13                      = 2061
CG_TEXUINT14                      = 2062
CG_TEXUINT15                      = 2063

CG_ATTR0                          = 2113
CG_ATTR1                          = 2114
CG_ATTR2                          = 2115
CG_ATTR3                          = 2116
CG_ATTR4                          = 2117
CG_ATTR5                          = 2118
CG_ATTR6                          = 2119
CG_ATTR7                          = 2120
CG_ATTR8                          = 2121
CG_ATTR9                          = 2122
CG_ATTR10                         = 2123
CG_ATTR11                         = 2124
CG_ATTR12                         = 2125
CG_ATTR13                         = 2126
CG_ATTR14                         = 2127
CG_ATTR15                         = 2128

CG_C                              = 2178

CG_TEX0                           = 2179
CG_TEX1                           = 2180
CG_TEX2                           = 2181
CG_TEX3                           = 2192 # uff, another bug report
CG_TEX4                           = 2193
CG_TEX5                           = 2194
CG_TEX6                           = 2195
CG_TEX7                           = 2196

CG_HPOS                           = 2243
CG_COL0                           = 2245
CG_COL1                           = 2246
CG_COL2                           = 2247
CG_COL3                           = 2248
CG_PSIZ                           = 2309
CG_WPOS                           = 2373

CG_POSITION0                      = 2437
CG_POSITION1                      = 2438
CG_POSITION2                      = 2439
CG_POSITION3                      = 2440
CG_POSITION4                      = 2441
CG_POSITION5                      = 2442
CG_POSITION6                      = 2443
CG_POSITION7                      = 2444
CG_POSITION8                      = 2445
CG_POSITION9                      = 2446
CG_POSITION10                     = 2447
CG_POSITION11                     = 2448
CG_POSITION12                     = 2449
CG_POSITION13                     = 2450
CG_POSITION14                     = 2451
CG_POSITION15                     = 2452

CG_DIFFUSE0                       = 2501

CG_TANGENT0                       = 2565
CG_TANGENT1                       = 2566
CG_TANGENT2                       = 2567
CG_TANGENT3                       = 2568
CG_TANGENT4                       = 2569
CG_TANGENT5                       = 2570
CG_TANGENT6                       = 2571
CG_TANGENT7                       = 2572
CG_TANGENT8                       = 2573
CG_TANGENT9                       = 2574
CG_TANGENT10                      = 2575
CG_TANGENT11                      = 2576
CG_TANGENT12                      = 2577
CG_TANGENT13                      = 2578
CG_TANGENT14                      = 2579
CG_TANGENT15                      = 2580

CG_SPECULAR0                      = 2629

CG_BLENDINDICES0                  = 2693
CG_BLENDINDICES1                  = 2694
CG_BLENDINDICES2                  = 2695
CG_BLENDINDICES3                  = 2696
CG_BLENDINDICES4                  = 2697
CG_BLENDINDICES5                  = 2698
CG_BLENDINDICES6                  = 2699
CG_BLENDINDICES7                  = 2700
CG_BLENDINDICES8                  = 2701
CG_BLENDINDICES9                  = 2702
CG_BLENDINDICES10                 = 2703
CG_BLENDINDICES11                 = 2704
CG_BLENDINDICES12                 = 2705
CG_BLENDINDICES13                 = 2706
CG_BLENDINDICES14                 = 2707
CG_BLENDINDICES15                 = 2708

CG_COLOR0                         = 2757
CG_COLOR1                         = 2758
CG_COLOR2                         = 2759
CG_COLOR3                         = 2760
CG_COLOR4                         = 2761
CG_COLOR5                         = 2762
CG_COLOR6                         = 2763
CG_COLOR7                         = 2764
CG_COLOR8                         = 2765
CG_COLOR9                         = 2766
CG_COLOR10                        = 2767
CG_COLOR11                        = 2768
CG_COLOR12                        = 2769
CG_COLOR13                        = 2770
CG_COLOR14                        = 2771
CG_COLOR15                        = 2772

CG_PSIZE0                         = 2821
CG_PSIZE1                         = 2822
CG_PSIZE2                         = 2823
CG_PSIZE3                         = 2824
CG_PSIZE4                         = 2825
CG_PSIZE5                         = 2826
CG_PSIZE6                         = 2827
CG_PSIZE7                         = 2828
CG_PSIZE8                         = 2829
CG_PSIZE9                         = 2830
CG_PSIZE10                        = 2831
CG_PSIZE11                        = 2832
CG_PSIZE12                        = 2833
CG_PSIZE13                        = 2834
CG_PSIZE14                        = 2835
CG_PSIZE15                        = 2836

CG_BINORMAL0                      = 2885
CG_BINORMAL1                      = 2886
CG_BINORMAL2                      = 2887
CG_BINORMAL3                      = 2888
CG_BINORMAL4                      = 2889
CG_BINORMAL5                      = 2890
CG_BINORMAL6                      = 2891
CG_BINORMAL7                      = 2892
CG_BINORMAL8                      = 2893
CG_BINORMAL9                      = 2894
CG_BINORMAL10                     = 2895
CG_BINORMAL11                     = 2896
CG_BINORMAL12                     = 2897
CG_BINORMAL13                     = 2898
CG_BINORMAL14                     = 2899
CG_BINORMAL15                     = 2900

CG_FOG0                           = 2917
CG_FOG1                           = 2918
CG_FOG2                           = 2919
CG_FOG3                           = 2920
CG_FOG4                           = 2921
CG_FOG5                           = 2922
CG_FOG6                           = 2923
CG_FOG7                           = 2924
CG_FOG8                           = 2925
CG_FOG9                           = 2926
CG_FOG1                           = 2927
CG_FOG1                           = 2928
CG_FOG1                           = 2929
CG_FOG1                           = 2930
CG_FOG1                           = 2931
CG_FOG1                           = 2932

CG_DEPTH0                         = 2933
CG_DEPTH1                         = 2934
CG_DEPTH2                         = 2935
CG_DEPTH3                         = 2936
CG_DEPTH4                         = 2937
CG_DEPTH5                         = 2938
CG_DEPTH6                         = 2939
CG_DEPTH7                         = 2940
CG_DEPTH8                         = 2941
CG_DEPTH9                         = 29542  # uff, submitted a bug report to nvidia
CG_DEPTH10                        = 2943
CG_DEPTH11                        = 2944
CG_DEPTH12                        = 2945
CG_DEPTH13                        = 2946
CG_DEPTH14                        = 2947
CG_DEPTH15                        = 2948

CG_SAMPLE0                        = 2949
CG_SAMPLE1                        = 2950
CG_SAMPLE2                        = 2951
CG_SAMPLE3                        = 2952
CG_SAMPLE4                        = 2953
CG_SAMPLE5                        = 2954
CG_SAMPLE6                        = 2955
CG_SAMPLE7                        = 2956
CG_SAMPLE8                        = 2957
CG_SAMPLE9                        = 2958
CG_SAMPLE10                       = 2959
CG_SAMPLE11                       = 2960
CG_SAMPLE12                       = 2961
CG_SAMPLE13                       = 2962
CG_SAMPLE14                       = 2963
CG_SAMPLE15                       = 2964

CG_BLENDWEIGHT0                   = 3028
CG_BLENDWEIGHT1                   = 3029
CG_BLENDWEIGHT2                   = 3030
CG_BLENDWEIGHT3                   = 3031
CG_BLENDWEIGHT4                   = 3032
CG_BLENDWEIGHT5                   = 3033
CG_BLENDWEIGHT6                   = 3034
CG_BLENDWEIGHT7                   = 3035
CG_BLENDWEIGHT8                   = 3036
CG_BLENDWEIGHT9                   = 3037
CG_BLENDWEIGHT10                  = 3038
CG_BLENDWEIGHT11                  = 3039
CG_BLENDWEIGHT12                  = 3040
CG_BLENDWEIGHT13                  = 3041
CG_BLENDWEIGHT14                  = 3042
CG_BLENDWEIGHT15                  = 3043


CG_NORMAL0                        = 3092
CG_NORMAL1                        = 3093
CG_NORMAL2                        = 3094
CG_NORMAL3                        = 3095
CG_NORMAL4                        = 3096
CG_NORMAL5                        = 3097
CG_NORMAL6                        = 3098
CG_NORMAL7                        = 3099
CG_NORMAL8                        = 3100
CG_NORMAL9                        = 3101
CG_NORMAL10                       = 3102
CG_NORMAL11                       = 3103
CG_NORMAL12                       = 3104
CG_NORMAL13                       = 3105
CG_NORMAL14                       = 3106
CG_NORMAL15                       = 3107

CG_FOGCOORD                       = 3156

CG_TEXCOORD0                      = 3220
CG_TEXCOORD1                      = 3221
CG_TEXCOORD2                      = 3222
CG_TEXCOORD3                      = 3223
CG_TEXCOORD4                      = 3224
CG_TEXCOORD5                      = 3225
CG_TEXCOORD6                      = 3226
CG_TEXCOORD7                      = 3227
CG_TEXCOORD8                      = 3228
CG_TEXCOORD9                      = 3229
CG_TEXCOORD10                     = 3230
CG_TEXCOORD11                     = 3231
CG_TEXCOORD12                     = 3232
CG_TEXCOORD13                     = 3233
CG_TEXCOORD14                     = 3234
CG_TEXCOORD15                     = 3235

CG_TESSFACTOR                     = 3255

CG_COMBINER_CONST0                = 3284
CG_COMBINER_CONST1                = 3285

CG_COMBINER_STAGE_CONST0          = 3286
CG_COMBINER_STAGE_CONST1          = 3287
CG_OFFSET_TEXTURE_MATRIX          = 3288
CG_OFFSET_TEXTURE_SCALE           = 3289
CG_OFFSET_TEXTURE_BIAS            = 3290
CG_CONST_EYE                      = 3291

CG_UNKNOWN                        = 4096
CG_IN                             = 4097
CG_OUT                            = 4098
CG_INOUT                          = 4099
CG_MIXED                          = 4100
CG_VARYING                        = 4101
CG_UNIFORM                        = 4102
CG_CONSTANT                       = 4103
CG_PROGRAM_SOURCE                 = 4104
CG_PROGRAM_ENTRY                  = 4105
CG_COMPILED_PROGRAM               = 4106
CG_PROGRAM_PROFILE                = 4107
CG_GLOBAL                         = 4108
CG_PROGRAM                        = 4109
CG_DEFAULT                        = 4110
CG_ERROR                          = 4111
CG_SOURCE                         = 4112
CG_OBJECT                         = 4113

CG_PROFILE_START                  = 6144
CG_PROFILE_UNKNOWN                = 6145
CG_PROFILE_VP20                   = 6146
CG_PROFILE_FP20                   = 6147
CG_PROFILE_VP30                   = 6148
CG_PROFILE_FP30                   = 6149
CG_PROFILE_ARBVP1                 = 6150
CG_PROFILE_ARBFP1                 = 7000
CG_PROFILE_VS_1_1                 = 6153
CG_PROFILE_VS_2_0                 = 6154
CG_PROFILE_VS_2_X                 = 6155
CG_PROFILE_PS_1_1                 = 6159
CG_PROFILE_PS_1_2                 = 6160
CG_PROFILE_PS_1_3                 = 6161
CG_PROFILE_PS_2_0                 = 6162
CG_PROFILE_PS_2_X                 = 6163

CG_NO_ERROR                       = 0
CG_COMPILER_ERROR                 = 1
CG_INVALID_PARAMETER_ERROR        = 2
CG_INVALID_PROFILE_ERROR          = 3
CG_PROGRAM_LOAD_ERROR             = 4
CG_PROGRAM_BIND_ERROR             = 5
CG_PROGRAM_NOT_LOADED_ERROR       = 6
CG_UNSUPPORTED_GL_EXTENSION_ERROR = 7
CG_INVALID_VALUE_TYPE_ERROR       = 8
CG_NOT_MATRIX_PARAM_ERROR         = 9
CG_INVALID_ENUMERANT_ERROR        = 10
CG_NOT_4x4_MATRIX_ERROR           = 11
CG_FILE_READ_ERROR                = 12
CG_FILE_WRITE_ERROR               = 13
CG_NVPARSE_ERROR                  = 14
CG_MEMORY_ALLOC_ERROR             = 15
CG_INVALID_CONTEXT_HANDLE_ERROR   = 16
CG_INVALID_PARAM_HANDLE_ERROR     = 17
CG_UNKNOWN_PROFILE_ERROR          = 18
CG_VAR_ARG_ERROR                  = 19
CG_INVALID_DIMENSION_ERROR        = 20
CG_ARRAY_PARAM_ERROR              = 21
CG_OUT_OF_ARRAY_BOUNDS_ERROR      = 22

CG_GL_MATRIX_IDENTITY             = 0
CG_GL_MATRIX_TRANSPOSE            = 1
CG_GL_MATRIX_INVERSE              = 2
CG_GL_MATRIX_INVERSE_TRANSPOSE    = 3

CG_GL_MODELVIEW_MATRIX            = 4
CG_GL_PROJECTION_MATRIX           = 5
CG_GL_TEXTURE_MATRIX              = 6
CG_GL_MODELVIEW_PROJECTION_MATRIX = 7

CG_GL_VERTEX                      = 8
CG_GL_FRAGMENT                    = 9

#include "frame_home.h"
#include <HTTPClient.h>

HTTPClient http;

struct _ac_ctrl{
 String mode;
 uint8_t temp;
 String fan;
};

std::map<String, struct _ac_ctrl> ac_ctrl_init ={
  {"liv", {"cool", 26, "med"} },
  {"bed",  {"cool", 26, "med"} },
  {"baby",  {"cool", 26, "med"} },
};

std::map<String, struct _ac_ctrl> ac_ctrl_current = ac_ctrl_init;

struct _ac_loc_info{
 String maker;
 String ipaddr;
};

String loc_list[]={"liv", "baby", "bed"};
std::map<String, struct _ac_loc_info> ac_loc_info ={
  {loc_list[0], {"sharp",  "192.168.0.14"}},
  {loc_list[1], {"corona", "192.168.0.30"}},
  {loc_list[2], {"sharp",  "192.168.0.83"}},
};

std::map<String, int> ac_mode_to_canvas = {
    {"off", 0},
    {"cool", 1},
    {"dry", 2},
    {"heat", 3},
};

void Frame_Home::InitSwitch(EPDGUI_Switch* sw, String title, String subtitle, const uint8_t *img1, const uint8_t *img2)
{
    memcpy(sw->Canvas(0)->frameBuffer(), ImageResource_home_button_background_228x228, 228 * 228 / 2);
    sw->Canvas(0)->setTextSize(36);
    sw->Canvas(0)->setTextDatum(TC_DATUM);
    sw->Canvas(0)->drawString(title, 114, 136);
    sw->Canvas(0)->setTextSize(26);
    sw->Canvas(0)->drawString(subtitle, 114, 183);
    memcpy(sw->Canvas(1)->frameBuffer(), sw->Canvas(0)->frameBuffer(), 228 * 228 / 2);
    sw->Canvas(0)->pushImage(68, 20, 92, 92, img1);
    sw->Canvas(1)->pushImage(68, 20, 92, 92, img2);
}

void air_control(EPDGUI_Switch &sw, String mode, int temp)
{
    String maker = "";
    String ipaddr = "";
    String found_loc= "";
    for (auto loc : loc_list){
        if (sw.GetCustomString().indexOf(loc)>=0){
            maker = ac_loc_info[loc].maker;
            ipaddr = ac_loc_info[loc].ipaddr;
            found_loc = loc;
            break;
        } 
    }
    if (mode == "off"){
        // http.begin("http://"+ ipaddr + "/ac_off?maker=" + maker);
        // http.GET();
        // http.end();     
        return;
    }
    ac_ctrl_current[found_loc].mode = mode;
    ac_ctrl_current[found_loc].temp = temp;
    // http.begin("http://" + ipaddr + "/ac_on?maker=" + maker + "&mode=" + ac_ctrl_current[found_loc].mode + "&temp=" + ac_ctrl_current[found_loc].temp +"&fan=" + ac_ctrl_current[found_loc].fan );
    // http.GET();
    // http.end();
}

void display_ac_status( EPDGUI_Switch &sw, String loc, int temp )
{
    char buf[32];
    ac_ctrl_current[loc].temp = temp;
    sprintf(buf, "%s %d℃", ac_ctrl_current[loc].mode.c_str(), temp);
    int idx = ac_mode_to_canvas[ac_ctrl_current[loc].mode]; 
    //Serial.printf("*** %s mode(%s) idx =%d\n", loc.c_str(), ac_ctrl_current[loc].mode.c_str(), idx);
    sw.Canvas(idx)->setTextSize(36);
    sw.Canvas(idx)->setTextDatum(TC_DATUM);
    sw.Canvas(idx)->fillRect(114 - 100, 108, 200, 38, 0);
    sw.Canvas(idx)->drawString(buf, 114, 108);
    //sw.Canvas(idx)->drawString(ac_ctrl_current[loc].mode, 170, 92);
    sw.Canvas(idx)->pushCanvas(sw.getX(), sw.getY(), UPDATE_MODE_A2);
}


void key_home_air_adjust_cb(epdgui_args_vector_t &args)
{
    int operation = ((EPDGUI_Button*)(args[0]))->GetCustomString().toInt();
    EPDGUI_Switch *sw = ((EPDGUI_Switch*)(args[1]));
    String loc = sw->GetCustomString();
    if(sw->getState() == 0)
    {
        return;
    }
    int temp = ac_ctrl_current[loc].temp;
    if(operation == 1)
    {
        temp++;
    }
    else
    {
        temp--;
    }
    air_control(*sw, ac_ctrl_current[loc].mode, (uint8_t)temp);
    display_ac_status(*sw, loc, temp);
}


void key_home_air_state0_cb(epdgui_args_vector_t &args)
{
    EPDGUI_Button *b1 = ((EPDGUI_Button*)(args[0]));
    EPDGUI_Button *b2 = ((EPDGUI_Button*)(args[1]));
    EPDGUI_Switch *sw = ((EPDGUI_Switch*)(args[2]));
    String loc = sw->GetCustomString();
    air_control(*sw, "off", ac_ctrl_current[loc].temp);
    b1->SetEnable(false);
    b2->SetEnable(false);
}
void key_home_air_state1_cb(epdgui_args_vector_t &args)
{
    EPDGUI_Button *b1 = ((EPDGUI_Button*)(args[0]));
    EPDGUI_Button *b2 = ((EPDGUI_Button*)(args[1]));
    EPDGUI_Switch *sw = ((EPDGUI_Switch*)(args[2]));
    String loc = sw->GetCustomString();
    air_control(*sw, "cool", ac_ctrl_current[loc].temp);
    display_ac_status(*sw, loc, ac_ctrl_current[loc].temp);

    b1->SetEnable(true);
    b2->SetEnable(true);
}

void key_home_air_state2_cb(epdgui_args_vector_t &args)
{
    EPDGUI_Button *b1 = ((EPDGUI_Button*)(args[0]));
    EPDGUI_Button *b2 = ((EPDGUI_Button*)(args[1]));
    EPDGUI_Switch *sw = ((EPDGUI_Switch*)(args[2]));
    String loc = sw->GetCustomString();
    air_control(*sw, "dry", ac_ctrl_current[loc].temp);
    display_ac_status(*sw, loc, ac_ctrl_current[loc].temp);

    b1->SetEnable(true);
    b2->SetEnable(true);
}

void key_home_air_state3_cb(epdgui_args_vector_t &args)
{
    EPDGUI_Button *b1 = ((EPDGUI_Button*)(args[0]));
    EPDGUI_Button *b2 = ((EPDGUI_Button*)(args[1]));
    EPDGUI_Switch *sw = ((EPDGUI_Switch*)(args[2]));
    String loc = sw->GetCustomString();
    air_control(*sw, "heat", ac_ctrl_current[loc].temp);
    display_ac_status(*sw, loc, ac_ctrl_current[loc].temp);

    b1->SetEnable(true);
    b2->SetEnable(true);
}


Frame_Home::Frame_Home(void)
{
    _frame_name = "Frame_Home";

    _sw_light1       = new EPDGUI_Switch(2, 20, 44 + 72, 228, 228);
    _sw_light2       = new EPDGUI_Switch(2, 20, 324 + 72, 228, 228);
    _sw_light3       = new EPDGUI_Switch(2, 20, 604 + 72, 228, 228);
    _sw_air_1        = new EPDGUI_Switch(4, 288, 44 + 72, 228, 184);
    _sw_air_2        = new EPDGUI_Switch(4, 288, 324 + 72, 228, 184);
    _sw_air_3        = new EPDGUI_Switch(4, 288, 604 + 72, 228, 184);
    _key_air_1_plus  = new EPDGUI_Button(288 + 116, 44 + 72 + 184, 112, 44);
    _key_air_1_minus = new EPDGUI_Button(288, 44 + 72 + 184, 116, 44);
    _key_air_2_plus  = new EPDGUI_Button(288 + 116, 324 + 72 + 184, 112, 44);
    _key_air_2_minus = new EPDGUI_Button(288, 324 + 72 + 184, 116, 44);
    _key_air_3_plus  = new EPDGUI_Button(288 + 116, 604 + 72 + 184, 112, 44);
    _key_air_3_minus = new EPDGUI_Button(288, 604 + 72 + 184, 116, 44);

    _key_air_1_plus ->SetCustomString("1");
    _key_air_1_minus->SetCustomString("0");
    _key_air_2_plus ->SetCustomString("1");
    _key_air_2_minus->SetCustomString("0");
    _key_air_3_plus ->SetCustomString("1");
    _key_air_3_minus->SetCustomString("0");

    _key_air_1_plus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, _key_air_1_plus);
    _key_air_1_plus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, _sw_air_1);
    _key_air_1_plus->Bind(EPDGUI_Button::EVENT_RELEASED, key_home_air_adjust_cb);
    _key_air_1_minus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, _key_air_1_minus);
    _key_air_1_minus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, _sw_air_1);
    _key_air_1_minus->Bind(EPDGUI_Button::EVENT_RELEASED, key_home_air_adjust_cb);
    _key_air_2_plus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, _key_air_2_plus);
    _key_air_2_plus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, _sw_air_2);
    _key_air_2_plus->Bind(EPDGUI_Button::EVENT_RELEASED, key_home_air_adjust_cb);
    _key_air_2_minus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, _key_air_2_minus);
    _key_air_2_minus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, _sw_air_2);
    _key_air_2_minus->Bind(EPDGUI_Button::EVENT_RELEASED, key_home_air_adjust_cb);
    _key_air_3_plus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, _key_air_3_plus);
    _key_air_3_plus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, _sw_air_3);
    _key_air_3_plus->Bind(EPDGUI_Button::EVENT_RELEASED, key_home_air_adjust_cb);
    _key_air_3_minus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, _key_air_3_minus);
    _key_air_3_minus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, _sw_air_3);
    _key_air_3_minus->Bind(EPDGUI_Button::EVENT_RELEASED, key_home_air_adjust_cb);

    M5EPD_Canvas canvas_temp(&M5.EPD);
    canvas_temp.createRender(36);
    uint8_t language = GetLanguage();

    if(language == LANGUAGE_JA)
    {
        InitSwitch(_sw_light1, "ランプ", "客間", ImageResource_home_icon_light_off_92x92, ImageResource_home_icon_light_on_92x92);
        InitSwitch(_sw_light2, "ランプ", "寝室", ImageResource_home_icon_light_off_92x92, ImageResource_home_icon_light_on_92x92);
        InitSwitch(_sw_light3," ランプ", "Baby", ImageResource_home_icon_light_off_92x92, ImageResource_home_icon_light_on_92x92);
    }
    else if(language == LANGUAGE_ZH)
    {
        InitSwitch(_sw_light1, "吸顶灯", "客厅", ImageResource_home_icon_light_off_92x92, ImageResource_home_icon_light_on_92x92);
        InitSwitch(_sw_light2, "台灯", "卧室", ImageResource_home_icon_light_off_92x92, ImageResource_home_icon_light_on_92x92);
        InitSwitch(_sw_light3," ランプ", "Baby", ImageResource_home_icon_light_off_92x92, ImageResource_home_icon_light_on_92x92);
    }
    else
    {
        InitSwitch(_sw_light1, "Ceiling Light", "Living Room", ImageResource_home_icon_light_off_92x92, ImageResource_home_icon_light_on_92x92);
        InitSwitch(_sw_light2, "Ceiling Light", "Bedroom", ImageResource_home_icon_light_off_92x92, ImageResource_home_icon_light_on_92x92);
        InitSwitch(_sw_light3," Ceiling Light", "Babyroom", ImageResource_home_icon_light_off_92x92, ImageResource_home_icon_light_on_92x92);
    }

    memcpy(_sw_air_1->Canvas(0)->frameBuffer(), ImageResource_home_air_background_228x184, 228 * 184 / 2);
    _sw_air_1->Canvas(0)->setTextDatum(TC_DATUM);
    _sw_air_1->Canvas(0)->setTextSize(26);
    if(language == LANGUAGE_JA)
    {
        _sw_air_1->Canvas(0)->drawString("客間", 114, 152);
    }
    else if(language == LANGUAGE_ZH)
    {
        _sw_air_1->Canvas(0)->drawString("客厅", 114, 152);
    }
    else
    {
        _sw_air_1->Canvas(0)->drawString("Living Room", 114, 152);
    }
    memcpy(_sw_air_1->Canvas(1)->frameBuffer(), _sw_air_1->Canvas(0)->frameBuffer(), 228 * 184 / 2);
    _sw_air_1->Canvas(0)->setTextSize(36);
    _sw_air_1->Canvas(0)->drawString("OFF", 114, 108);
    _sw_air_1->Canvas(1)->setTextSize(36);
    _sw_air_1->Canvas(1)->setTextDatum(TC_DATUM);
    _sw_air_1->Canvas(1)->drawString("cool 26℃", 114, 108);
    _sw_air_1->SetCustomString("liv");

    memcpy(_sw_air_1->Canvas(2)->frameBuffer(), _sw_air_1->Canvas(0)->frameBuffer(), 228 * 184 / 2);
    _sw_air_1->Canvas(2)->setTextSize(36);
    _sw_air_1->Canvas(2)->setTextDatum(TC_DATUM);
    _sw_air_1->Canvas(2)->drawString("dry 26℃", 114, 108);

    memcpy(_sw_air_1->Canvas(3)->frameBuffer(), _sw_air_1->Canvas(0)->frameBuffer(), 228 * 184 / 2);
    _sw_air_1->Canvas(3)->setTextSize(36);
    _sw_air_1->Canvas(3)->setTextDatum(TC_DATUM);
    _sw_air_1->Canvas(3)->drawString("heat 26℃", 114, 108);

    memcpy(_sw_air_2->Canvas(0)->frameBuffer(), ImageResource_home_air_background_228x184, 228 * 184 / 2);
    _sw_air_2->Canvas(0)->setTextDatum(TC_DATUM);
    _sw_air_2->Canvas(0)->setTextSize(26);
    if(language == LANGUAGE_JA)
    {
        _sw_air_2->Canvas(0)->drawString("寝室", 114, 152);
    }
    else if(language == LANGUAGE_ZH)
    {
        _sw_air_2->Canvas(0)->drawString("卧室", 114, 152);
    }
    else
    {
        _sw_air_2->Canvas(0)->drawString("Bedroom", 114, 152);
    }
    memcpy(_sw_air_2->Canvas(1)->frameBuffer(), _sw_air_2->Canvas(0)->frameBuffer(), 228 * 184 / 2);
    _sw_air_2->Canvas(0)->setTextSize(36);
    _sw_air_2->Canvas(0)->drawString("OFF", 114, 108);
    _sw_air_2->Canvas(1)->setTextSize(36);
    _sw_air_2->Canvas(1)->setTextDatum(TC_DATUM);
    _sw_air_2->Canvas(1)->drawString("cool 26℃", 114, 108);
    _sw_air_2->SetCustomString("bed");

    memcpy(_sw_air_2->Canvas(2)->frameBuffer(), _sw_air_2->Canvas(0)->frameBuffer(), 228 * 184 / 2);
    _sw_air_2->Canvas(2)->setTextSize(36);
    _sw_air_2->Canvas(2)->setTextDatum(TC_DATUM);
    _sw_air_2->Canvas(2)->drawString("dry 26℃", 114, 108);

    memcpy(_sw_air_2->Canvas(3)->frameBuffer(), _sw_air_2->Canvas(0)->frameBuffer(), 228 * 184 / 2);
    _sw_air_2->Canvas(3)->setTextSize(36);
    _sw_air_2->Canvas(3)->setTextDatum(TC_DATUM);
    _sw_air_2->Canvas(3)->drawString("heat 26℃", 114, 108);


    memcpy(_sw_air_3->Canvas(0)->frameBuffer(), ImageResource_home_air_background_228x184, 228 * 184 / 2);
    _sw_air_3->Canvas(0)->setTextDatum(TC_DATUM);
    _sw_air_3->Canvas(0)->setTextSize(26);
    if(language == LANGUAGE_JA)
    {
        _sw_air_3->Canvas(0)->drawString("Baby", 114, 152);
    }
    else if(language == LANGUAGE_ZH)
    {
        _sw_air_3->Canvas(0)->drawString("Baby", 114, 152);
    }
    else
    {
        _sw_air_3->Canvas(0)->drawString("Baby", 114, 152);
    }
    memcpy(_sw_air_3->Canvas(1)->frameBuffer(), _sw_air_3->Canvas(0)->frameBuffer(), 228 * 184 / 2);
    _sw_air_3->Canvas(0)->setTextSize(36);
    _sw_air_3->Canvas(0)->drawString("OFF", 114, 108);
    _sw_air_3->Canvas(1)->setTextSize(36);
    _sw_air_3->Canvas(1)->setTextDatum(TC_DATUM);
    _sw_air_3->Canvas(1)->drawString("cool 26℃", 114, 108);
    _sw_air_3->SetCustomString("baby");

    memcpy(_sw_air_3->Canvas(2)->frameBuffer(), _sw_air_3->Canvas(0)->frameBuffer(), 228 * 184 / 2);
    _sw_air_3->Canvas(2)->setTextSize(36);
    _sw_air_3->Canvas(2)->setTextDatum(TC_DATUM);
    _sw_air_3->Canvas(2)->drawString("dry 26℃", 114, 108);

    memcpy(_sw_air_3->Canvas(3)->frameBuffer(), _sw_air_3->Canvas(0)->frameBuffer(), 228 * 184 / 2);
    _sw_air_3->Canvas(3)->setTextSize(36);
    _sw_air_3->Canvas(3)->setTextDatum(TC_DATUM);
    _sw_air_3->Canvas(3)->drawString("heat 26℃", 114, 108);


    memcpy(_key_air_1_plus->CanvasNormal()->frameBuffer(), ImageResource_home_air_background_r_112x44, 112 * 44 / 2);
    memcpy(_key_air_1_plus->CanvasPressed()->frameBuffer(), _key_air_1_plus->CanvasNormal()->frameBuffer(), 112 * 44 / 2);
    _key_air_1_plus->CanvasPressed()->ReverseColor();
    memcpy(_key_air_2_plus->CanvasNormal()->frameBuffer(), ImageResource_home_air_background_r_112x44, 112 * 44 / 2);
    memcpy(_key_air_2_plus->CanvasPressed()->frameBuffer(), _key_air_2_plus->CanvasNormal()->frameBuffer(), 112 * 44 / 2);
    _key_air_2_plus->CanvasPressed()->ReverseColor();
    memcpy(_key_air_3_plus->CanvasNormal()->frameBuffer(), ImageResource_home_air_background_r_112x44, 112 * 44 / 2);
    memcpy(_key_air_3_plus->CanvasPressed()->frameBuffer(), _key_air_3_plus->CanvasNormal()->frameBuffer(), 112 * 44 / 2);
    _key_air_3_plus->CanvasPressed()->ReverseColor();
    memcpy(_key_air_1_minus->CanvasNormal()->frameBuffer(), ImageResource_home_air_background_l_116x44, 116 * 44 / 2);
    memcpy(_key_air_1_minus->CanvasPressed()->frameBuffer(), _key_air_1_minus->CanvasNormal()->frameBuffer(), 116 * 44 / 2);
    _key_air_1_minus->CanvasPressed()->ReverseColor();
    memcpy(_key_air_2_minus->CanvasNormal()->frameBuffer(), ImageResource_home_air_background_l_116x44, 116 * 44 / 2);
    memcpy(_key_air_2_minus->CanvasPressed()->frameBuffer(), _key_air_2_minus->CanvasNormal()->frameBuffer(), 116 * 44 / 2);
    _key_air_2_minus->CanvasPressed()->ReverseColor();
    memcpy(_key_air_3_minus->CanvasNormal()->frameBuffer(), ImageResource_home_air_background_l_116x44, 116 * 44 / 2);
    memcpy(_key_air_3_minus->CanvasPressed()->frameBuffer(), _key_air_3_minus->CanvasNormal()->frameBuffer(), 116 * 44 / 2);
    _key_air_3_minus->CanvasPressed()->ReverseColor();

    _key_air_1_plus->SetEnable(false);
    _key_air_2_plus->SetEnable(false);
    _key_air_3_plus->SetEnable(false);
    _key_air_1_minus->SetEnable(false);
    _key_air_2_minus->SetEnable(false);
    _key_air_3_minus->SetEnable(false);

    _sw_air_1->Canvas(0)->pushImage(68, 12, 92, 92, ImageResource_home_icon_conditioner_off_92x92);
    _sw_air_1->Canvas(1)->pushImage(68, 12, 92, 92, ImageResource_home_icon_conditioner_on_92x92);
    _sw_air_1->Canvas(2)->pushImage(68, 12, 92, 92, ImageResource_home_icon_conditioner_on_92x92);
    _sw_air_1->Canvas(3)->pushImage(68, 12, 92, 92, ImageResource_home_icon_conditioner_on_92x92);
    _sw_air_2->Canvas(0)->pushImage(68, 12, 92, 92, ImageResource_home_icon_conditioner_off_92x92);
    _sw_air_2->Canvas(1)->pushImage(68, 12, 92, 92, ImageResource_home_icon_conditioner_on_92x92);
    _sw_air_2->Canvas(2)->pushImage(68, 12, 92, 92, ImageResource_home_icon_conditioner_on_92x92);
    _sw_air_2->Canvas(3)->pushImage(68, 12, 92, 92, ImageResource_home_icon_conditioner_on_92x92);
    _sw_air_3->Canvas(0)->pushImage(68, 12, 92, 92, ImageResource_home_icon_conditioner_off_92x92);
    _sw_air_3->Canvas(1)->pushImage(68, 12, 92, 92, ImageResource_home_icon_conditioner_on_92x92);
    _sw_air_3->Canvas(2)->pushImage(68, 12, 92, 92, ImageResource_home_icon_conditioner_on_92x92);
    _sw_air_3->Canvas(3)->pushImage(68, 12, 92, 92, ImageResource_home_icon_conditioner_on_92x92);

    _sw_air_1->AddArgs(0, 0, _key_air_1_plus);
    _sw_air_1->AddArgs(0, 1, _key_air_1_minus);
    _sw_air_1->AddArgs(0, 2, _sw_air_1);
    _sw_air_1->Bind(0, key_home_air_state0_cb);
    _sw_air_1->AddArgs(1, 0, _key_air_1_plus);
    _sw_air_1->AddArgs(1, 1, _key_air_1_minus);
    _sw_air_1->AddArgs(1, 2, _sw_air_1);
    _sw_air_1->Bind(1, key_home_air_state1_cb);
    _sw_air_1->AddArgs(2, 0, _key_air_1_plus);
    _sw_air_1->AddArgs(2, 1, _key_air_1_minus);
    _sw_air_1->AddArgs(2, 2, _sw_air_1);
    _sw_air_1->Bind(2, key_home_air_state2_cb);
    _sw_air_1->AddArgs(3, 0, _key_air_1_plus);
    _sw_air_1->AddArgs(3, 1, _key_air_1_minus);
    _sw_air_1->AddArgs(3, 2, _sw_air_1);
    _sw_air_1->Bind(3, key_home_air_state3_cb);

    _sw_air_2->AddArgs(0, 0, _key_air_2_plus);
    _sw_air_2->AddArgs(0, 1, _key_air_2_minus);
    _sw_air_2->AddArgs(0, 2, _sw_air_2);
    _sw_air_2->Bind(0, key_home_air_state0_cb);
    _sw_air_2->AddArgs(1, 0, _key_air_2_plus);
    _sw_air_2->AddArgs(1, 1, _key_air_2_minus);
    _sw_air_2->AddArgs(1, 2, _sw_air_2);
    _sw_air_2->Bind(1, key_home_air_state1_cb);
    _sw_air_2->AddArgs(2, 0, _key_air_2_plus);
    _sw_air_2->AddArgs(2, 1, _key_air_2_minus);
    _sw_air_2->AddArgs(2, 2, _sw_air_2);
    _sw_air_2->Bind(2, key_home_air_state2_cb);
    _sw_air_2->AddArgs(3, 0, _key_air_2_plus);
    _sw_air_2->AddArgs(3, 1, _key_air_2_minus);
    _sw_air_2->AddArgs(3, 2, _sw_air_2);
    _sw_air_2->Bind(3, key_home_air_state3_cb);

    _sw_air_3->AddArgs(0, 0, _key_air_3_plus);
    _sw_air_3->AddArgs(0, 1, _key_air_3_minus);
    _sw_air_3->AddArgs(0, 2, _sw_air_3);
    _sw_air_3->Bind(0, key_home_air_state0_cb);
    _sw_air_3->AddArgs(1, 0, _key_air_3_plus);
    _sw_air_3->AddArgs(1, 1, _key_air_3_minus);
    _sw_air_3->AddArgs(1, 2, _sw_air_3);
    _sw_air_3->Bind(1, key_home_air_state1_cb);
    _sw_air_3->AddArgs(2, 0, _key_air_3_plus);
    _sw_air_3->AddArgs(2, 1, _key_air_3_minus);
    _sw_air_3->AddArgs(2, 2, _sw_air_3);
    _sw_air_3->Bind(2, key_home_air_state2_cb);
    _sw_air_3->AddArgs(3, 0, _key_air_3_plus);
    _sw_air_3->AddArgs(3, 1, _key_air_3_minus);
    _sw_air_3->AddArgs(3, 2, _sw_air_3);
    _sw_air_3->Bind(3, key_home_air_state3_cb);

    if(language == LANGUAGE_JA)
    {
        exitbtn("ホーム");
        _canvas_title->drawString("コントロールパネル", 270, 34);
    }
    else if(language == LANGUAGE_ZH)
    {
        exitbtn("主页");
        _canvas_title->drawString("控制面板", 270, 34);
    }
    else
    {
        exitbtn("Home");
        _canvas_title->drawString("Control Panel", 270, 34);
    }

    _key_exit->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)(&_is_run));
    _key_exit->Bind(EPDGUI_Button::EVENT_RELEASED, &Frame_Base::exit_cb);
}

Frame_Home::~Frame_Home(void)
{
    delete _sw_light1;
    delete _sw_light2;
    delete _sw_light3;
    delete _sw_air_1;
    delete _sw_air_2;
    delete _sw_air_3;
    delete _key_air_1_plus;
    delete _key_air_1_minus;
    delete _key_air_2_plus;
    delete _key_air_2_minus;
    delete _key_air_3_plus;
    delete _key_air_3_minus;
}

int Frame_Home::init(epdgui_args_vector_t &args)
{
    _is_run = 1;
    M5.EPD.Clear();
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    EPDGUI_AddObject(_sw_light1);
    EPDGUI_AddObject(_sw_light2);
    EPDGUI_AddObject(_sw_light3);
    EPDGUI_AddObject(_sw_air_1);
    EPDGUI_AddObject(_sw_air_2);
    EPDGUI_AddObject(_sw_air_3);
    EPDGUI_AddObject(_key_air_1_plus);
    EPDGUI_AddObject(_key_air_1_minus);
    EPDGUI_AddObject(_key_air_2_plus);
    EPDGUI_AddObject(_key_air_2_minus);
    EPDGUI_AddObject(_key_air_3_plus);
    EPDGUI_AddObject(_key_air_3_minus);
    EPDGUI_AddObject(_key_exit);

    if(WiFi.status() != WL_CONNECTED){
        for (int i = 0; WiFi.status() != WL_CONNECTED; i++)
        {
            if (i % 10 == 0) {
                WiFi.begin(GetWifiSSID().c_str(), GetWifiPassword().c_str());
            }
            delay(1000); // wait a second for connection
        }
    }
    return 3;
}
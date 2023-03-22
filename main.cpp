
#include "simple_yolo.hpp"

#if defined(_WIN32)
#	include <Windows.h>
#   include <wingdi.h>
#	include <Shlwapi.h>
#	pragma comment(lib, "shlwapi.lib")
#   pragma comment(lib, "ole32.lib")
#   pragma comment(lib, "gdi32.lib")
#	undef min
#	undef max
#else
#	include <dirent.h>
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <unistd.h>
#   include <stdarg.h>
#endif
#include "nlohmann/json.hpp"
#include <fstream>
using namespace std;


static const char* cocolabels[] = { "drone" };
static std::tuple<uint8_t, uint8_t, uint8_t> hsv2bgr(float h, float s, float v){
    const int h_i = static_cast<int>(h * 6);
    const float f = h * 6 - h_i;
    const float p = v * (1 - s);
    const float q = v * (1 - f*s);
    const float t = v * (1 - (1 - f) * s);
    float r, g, b;
    switch (h_i) {
    case 0:r = v; g = t; b = p;break;
    case 1:r = q; g = v; b = p;break;
    case 2:r = p; g = v; b = t;break;
    case 3:r = p; g = q; b = v;break;
    case 4:r = t; g = p; b = v;break;
    case 5:r = v; g = p; b = q;break;
    default:r = 1; g = 1; b = 1;break;}
    return make_tuple(static_cast<uint8_t>(b * 255), static_cast<uint8_t>(g * 255), static_cast<uint8_t>(r * 255));
}

static std::tuple<uint8_t, uint8_t, uint8_t> random_color(int id){
    float h_plane = ((((unsigned int)id << 2) ^ 0x937151) % 100) / 100.0f;;
    float s_plane = ((((unsigned int)id << 3) ^ 0x315793) % 100) / 100.0f;
    return hsv2bgr(h_plane, s_plane, 1);
}

static bool exists(const string& path){

#ifdef _WIN32
    return ::PathFileExistsA(path.c_str());
#else
    return access(path.c_str(), R_OK) == 0;
#endif
}

static string get_file_name(const string& path, bool include_suffix){

    if (path.empty()) return "";

    int p = path.rfind('/');
    int e = path.rfind('\\');
    p = std::max(p, e);
    p += 1;

    //include suffix
    if (include_suffix)
        return path.substr(p);

    int u = path.rfind('.');
    if (u == -1)
        return path.substr(p);

    if (u <= p) u = path.size();
    return path.substr(p, u - p);
}

static double timestamp_now_float() {
    return chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count() / 1000.0;
}

bool requires_model(const string& name) {

    auto onnx_file = cv::format("%s.onnx", name.c_str());
    if (!exists(onnx_file)) {
        printf("Auto download %s\n", onnx_file.c_str());
        system(cv::format("wget http://zifuture.com:1556/fs/25.shared/%s", onnx_file.c_str()).c_str());
    }

    bool isexists = exists(onnx_file);
    if (!isexists) {
        printf("Download %s failed\n", onnx_file.c_str());
    }
    return isexists;
}

static void inference_and_performance(int deviceid, const string& engine_file, SimpleYolo::Mode mode, SimpleYolo::Type type, const string& model_name){

    auto engine = SimpleYolo::create_infer(engine_file, type, deviceid, 0.4f, 0.5f);
    if(engine == nullptr){
        printf("Engine is nullptr\n");
        return;
    }

	vector<cv::String> files_;
	files_.reserve(10000);

    cv::glob("inference/*.jpg", files_, true);
	vector<string> files(files_.begin(), files_.end());
    
    vector<cv::Mat> images;
    for(int i = 0; i < files.size(); ++i){
        auto image = cv::imread(files[i]);
        images.emplace_back(image);
    }

    // warmup
    vector<shared_future<SimpleYolo::BoxArray>> boxes_array;
    for(int i = 0; i < 10; ++i)
        boxes_array = engine->commits(images);
    boxes_array.back().get();
    boxes_array.clear();
    
    /////////////////////////////////////////////////////////
    const int ntest = 100;
    auto begin_timer = timestamp_now_float();

    for(int i  = 0; i < ntest; ++i)
        boxes_array = engine->commits(images);
    
    // wait all result
    boxes_array.back().get();

    float inference_average_time = (timestamp_now_float() - begin_timer) / ntest / images.size();
    auto type_name = SimpleYolo::type_name(type);
    auto mode_name = SimpleYolo::mode_string(mode);
    printf("%s[%s] average: %.2f ms / image, FPS: %.2f\n", engine_file.c_str(), type_name, inference_average_time, 1000 / inference_average_time);
    std::ofstream out("1.txt",std::ios::out|std::ios::ate);
    using Json = nlohmann::json;
    
    string root = "simple_yolo_result";
    for(int i = 0; i < boxes_array.size(); ++i){

        auto& image = images[i];
        auto boxes  = boxes_array[i].get();
        Json all_out;
        for(auto& obj : boxes){
            uint8_t b, g, r;
            tie(b, g, r) = random_color(obj.class_label);
            cv::rectangle(image, cv::Point(obj.left, obj.top), cv::Point(obj.right, obj.bottom), cv::Scalar(b, g, r), 5);

            auto name    = cocolabels[obj.class_label];
            auto caption = cv::format("%s %.2f", name, obj.confidence);
            int width    = cv::getTextSize(caption, 0, 1, 2, nullptr).width + 10;
            cv::rectangle(image, cv::Point(obj.left-3, obj.top-33), cv::Point(obj.left + width, obj.top), cv::Scalar(b, g, r), -1);
            cv::putText(image, caption, cv::Point(obj.left, obj.top-5), 0, 1, cv::Scalar::all(0), 2, 16);
            Json::array_t it = {obj.left,obj.top,obj.right-obj.left,obj.bottom - obj.top};
            all_out["res"].push_back(it);
            
        }
        out << all_out.dump() << "\n\n";
        string file_name = get_file_name(files[i], false);
        string save_path = cv::format("%s/%s.jpg", root.c_str(), file_name.c_str());
        printf("Save to %s, %d object, average time %.2f ms\n", save_path.c_str(), boxes.size(), inference_average_time);
        cv::imwrite(save_path, image);
    }
    engine.reset();
}

static void test(SimpleYolo::Type type, SimpleYolo::Mode mode, const string& model){

    int deviceid = 0;
    auto mode_name = SimpleYolo::mode_string(mode);
    SimpleYolo::set_device(deviceid);

    const char* name = model.c_str();
    printf("===================== test %s %s %s ==================================\n", SimpleYolo::type_name(type), mode_name, name);

    if(!requires_model(name))
        return;

    string onnx_file = cv::format("%s.onnx", name);
    string model_file = cv::format("%s.%s.trtmodel", name, mode_name);
    int test_batch_size = 16;
    
    if(!exists(model_file)){
        SimpleYolo::compile(
            mode, type,                 // FP32、FP16、INT8
            test_batch_size,            // max batch size
            onnx_file,                  // source 
            model_file,                 // save to
            1 << 30,
            "inference"
        );
    }
    inference_and_performance(deviceid, model_file, mode, type, name);
}



int main(){


    test(SimpleYolo::Type::V7, SimpleYolo::Mode::FP16, "best");

    return 0;
}
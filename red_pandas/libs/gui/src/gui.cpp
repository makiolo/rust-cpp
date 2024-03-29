//
// Created by makiolo on 22-Dec-23.
//
#include "App.h"
#include <string.h>
#include <curl/curl.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <stdio.h>
#include <csv.h>
#include <fmt/format.h>
#include <implot_internal.h>
// #include <zmq.hpp>
// #include "producer.h"
// #include <future>
// #include <iostream>
// #include <string>
// #include <thread>

namespace fs = std::filesystem;

enum Interval
{
    Interval_Daily,
    Interval_Weekly,
    Interval_Monthly
};

class TickerData
{
public:

    TickerData() { }
    TickerData(std::string ticker) : ticker(ticker) { }

    void reserve(int n) {
        time.reserve(n);
        open.reserve(n);
        high.reserve(n);
        low.reserve(n);
        close.reserve(n);
        volume.reserve(n);
    }

    void push_back(double t, double o, double h, double l, double c, double v) {
        time.push_back(t);
        open.push_back(o);
        high.push_back(h);
        low.push_back(l);
        close.push_back(c);
        volume.push_back(v);

        int s = size();
        int i = std::max(0,s-20);
        double mean = ImMean(&close[i],std::min(s,20));
        double stdv = s > 1 ? ImStdDev(&close[i],std::min(s,20)) : 0;

        bollinger_top.push_back(mean - 2 * stdv);
        bollinger_mid.push_back(mean);
        bollinger_bot.push_back(mean + 2 * stdv);

    }

    int size() const {
        return (int)time.size();
    }

    std::string         ticker;
    std::vector<double> time;
    std::vector<double> open;
    std::vector<double> high;
    std::vector<double> low;
    std::vector<double> close;
    std::vector<double> volume;

    std::vector<double> bollinger_top;
    std::vector<double> bollinger_mid;
    std::vector<double> bollinger_bot;

};

class StockAPI
{
public:
    virtual ~StockAPI() {}
};

class YahooFinanceAPI
{
public:
    YahooFinanceAPI() {}

    TickerData get_ticker(std::string ticker, std::string start_date, std::string end_date, Interval interval)
    {
        //std::transform(ticker.begin(), ticker.end(),ticker.begin(), ::toupper);

        //std::string url = build_url(ticker, start_date, end_date, interval);
        //std::string filename = ticker + "_" + start_date + "_" + end_date + ".csv";
        ticker = "AAPL";
        std::string filename = "AAPL.csv";

        /*
        if (!fs::exists(filename)) {
            fmt::print("Downloading {}\n",filename);
            if (!download_file(url, filename))
            return TickerData("ERROR");
        }
        else {
            fmt::print("Reloading {}\n",filename);
        }
        */

        try {
            TickerData data(ticker);
            io::CSVReader<7> in(filename);
            in.read_header(io::ignore_extra_column, "Date", "Open", "High", "Low", "Close", "Adj Close", "Volume");
            std::string date; double o; double h; double l; double c; double ac; double v;
            while(in.read_row(date, o, h, l, c, ac, v)){
                double t = timestamp_from_string(date);
                data.push_back(t,o,h,l,c,v);
            }
            return data;
        }
        catch (...) {
            return TickerData("ERROR");
        }
    }

private:
    std::string build_url(std::string ticker, std::string start_date, std::string end_date, Interval interval)
    {
        static const std::string interval_str[]{"1d", "1wk", "1mo"};
        return fmt::format("https://query1.finance.yahoo.com/v7/finance/download/{}?period1={}&period2={}&interval={}&events=history",
                           ticker,
                           timestamp_from_string(start_date),
                           timestamp_from_string(end_date),
                           interval_str[interval]);
    }

    std::time_t timestamp_from_string(std::string date, const char *format = "%Y-%m-%d")
    {
        struct std::tm time = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        std::istringstream ss(date);
        ss >> std::get_time(&time, format);
        if (ss.fail())
        {
            std::cerr << "ERROR: Cannot parse date string (" << date << "); required format %Y-%m-%d" << std::endl;
            exit(1);
        }

        time.tm_hour = 0;
        time.tm_min = 0;
        time.tm_sec = 0;
#ifdef _WIN32
        return _mkgmtime(&time);
#else
        return timegm(&time);
#endif
    }

    bool download_file(std::string url, std::string filename)
    {
        bool success = false;
        CURL *curl;
        FILE *fp;
        CURLcode res;
        curl = curl_easy_init();
        if (curl)
        {
            fp = fopen(filename.c_str(), "wb");
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            res = curl_easy_perform(curl);
            if (res == 0)
                success = true;
            curl_easy_cleanup(curl);
            fclose(fp);
        }
        return success;
    }
};

template <typename T>
int BinarySearch(const T* arr, int l, int r, T x) {
    if (r >= l) {
        int mid = l + (r - l) / 2;
        if (arr[mid] == x)
            return mid;
        if (arr[mid] > x)
            return BinarySearch(arr, l, mid - 1, x);
        return BinarySearch(arr, mid + 1, r, x);
    }
    return -1;
}

void TickerTooltip(const TickerData& data, bool span_subplots = false) {
    ImDrawList* draw_list = ImPlot::GetPlotDrawList();
    const double half_width = 24*60*60*0.25*1.5;
    const bool hovered = span_subplots ? ImPlot::IsSubplotsHovered() : ImPlot::IsPlotHovered();
    if (hovered) {
        ImPlotPoint mouse   = ImPlot::GetPlotMousePos();
        mouse.x             = ImPlot::RoundTime(ImPlotTime::FromDouble(mouse.x), ImPlotTimeUnit_Day).ToDouble();
        float  tool_l       = ImPlot::PlotToPixels(mouse.x - half_width, mouse.y).x;
        float  tool_r       = ImPlot::PlotToPixels(mouse.x + half_width, mouse.y).x;
        float  tool_t       = ImPlot::GetPlotPos().y;
        float  tool_b       = tool_t + ImPlot::GetPlotSize().y;
        ImPlot::PushPlotClipRect();
        draw_list->AddRectFilled(ImVec2(tool_l, tool_t), ImVec2(tool_r, tool_b), IM_COL32(128,128,128,64));
        ImPlot::PopPlotClipRect();
        // find mouse location index
        int idx = BinarySearch(data.time.data(), 0, data.size() - 1, mouse.x);
        // render tool tip (won't be affected by plot clip rect)
        if (ImPlot::IsPlotHovered() && idx != -1) {
            ImGui::BeginTooltip();
            char buff[32];
            ImPlot::FormatDate(ImPlotTime::FromDouble(data.time[idx]),buff,32,ImPlotDateFmt_DayMoYr,ImPlot::GetStyle().UseISO8601);
            ImGui::Text("Date:");   ImGui::SameLine(60); ImGui::Text("%s",  buff);
            ImGui::Text("Open:");   ImGui::SameLine(60); ImGui::Text("$%.2f", data.open[idx]);
            ImGui::Text("Close:");  ImGui::SameLine(60); ImGui::Text("$%.2f", data.close[idx]);
            ImGui::Text("High:");   ImGui::SameLine(60); ImGui::Text("$%.2f", data.high[idx]);
            ImGui::Text("Low:");    ImGui::SameLine(60); ImGui::Text("$%.2f", data.low[idx]);
            // ImGui::Text("Volume:"); ImGui::SameLine(60); ImGui::Text(fmt::format(std::locale("es_ES.UTF-8"),"{:L}", (int)(data.volume[idx])).c_str());
            ImGui::EndTooltip();
        }
    }
}

void PlotOHLC(int timeline, const char* label_id, const TickerData& data, ImVec4 bullCol = ImVec4(0,1,0,1), ImVec4 bearCol = ImVec4(1,0,0,1)) {

    // get ImGui window DrawList
    ImDrawList* draw_list = ImPlot::GetPlotDrawList();
    // calc real value width
    const double half_width = 24*60*60*0.25;
    // begin plot item
    if (ImPlot::BeginItem(label_id)) {
        // override legend icon color
        ImPlot::GetCurrentItem()->Color = ImGui::GetColorU32(bullCol);
        // fit data if requested
        if (ImPlot::FitThisFrame()) {
            for (int i = 0; i < std::min(timeline, data.size()); ++i) {
                ImPlot::FitPoint(ImPlotPoint(data.time[i], data.low[i]));
                ImPlot::FitPoint(ImPlotPoint(data.time[i], data.high[i]));
            }
        }
        // render data
        for (int i = 0; i < std::min(timeline, data.size()); ++i) {

            /*
            fmt::print("data.close[i] = {}\n", data.close[i]);
            fmt::print("data.open[i] = {}\n", data.open[i]);
            fmt::print("data.low[i] = {}\n", data.low[i]);
            fmt::print("data.high[i] = {}\n", data.high[i]);
            */

            ImU32 color      = ImGui::GetColorU32(data.open[i] < data.close[i] ? bullCol: bearCol);
            ImVec2 open_pos  = ImPlot::PlotToPixels(data.time[i] - half_width, data.open[i]);
            ImVec2 close_pos = ImPlot::PlotToPixels(data.time[i] + half_width, data.close[i]);
            draw_list->AddRectFilled(close_pos, open_pos, color);
            ImVec2 low_pos   = ImPlot::PlotToPixels(data.time[i], data.low[i]);
            ImVec2 high_pos  = ImPlot::PlotToPixels(data.time[i], data.high[i]);
            draw_list->AddLine(low_pos,  high_pos , color, ImMax(1.0f, ImAbs(open_pos.x - close_pos.x)/10.0f));
        }

        int current = std::min(timeline, data.size());

        ImU32 color = ImGui::GetColorU32(bullCol);
        ImVec2 low_pos = ImPlot::PlotToPixels(data.time[current] - half_width, data.close[current] + half_width);
        ImVec2 high_pos = ImPlot::PlotToPixels(data.time[current] - half_width, data.close[current] - half_width);

        draw_list->AddRectFilled(low_pos, high_pos, color);

        // end plot item
        ImPlot::EndItem();
    }
}

int VolumeFormatter(double value, char* buff, int size, void*) {
    static double v[]      = {1000000000000,1000000000,1000000,1000,1};
    static const char* p[] = {"T","B","M","k",""};
    if (value == 0) {
        return snprintf(buff,size,"0");
    }
    for (int i = 0; i < 5; ++i) {
        if (fabs(value) >= v[i]) {
            return snprintf(buff,size,"%g%s",value/v[i],p[i]);
        }
    }
    return snprintf(buff,size,"%g%s",value/v[4],p[4]);
}

struct ImStocks : App
{
    using App::App;

    char t1_str[32];
    char t2_str[32];
    ImPlotTime t1;
    ImPlotTime t2;

    int timeline;

    void Start() override {
        t2 = ImPlot::FloorTime(ImPlotTime::FromDouble((double)time(0)),ImPlotTimeUnit_Day);
        t1 = ImPlot::AddTime(t2, ImPlotTimeUnit_Yr, -5);
        ImPlot::FormatDate(t1,t1_str,32,ImPlotDateFmt_DayMoYr,true);
        ImPlot::FormatDate(t2,t2_str,32,ImPlotDateFmt_DayMoYr,true);
        auto d = m_api.get_ticker("AAPL", t1_str, t2_str, Interval_Daily);
        if (d.ticker != "ERROR")
            m_ticker_data[d.ticker] = d;
        ImPlot::GetStyle().FitPadding.y = 0.2f;
        timeline = 8600;
    }

    void Update() override
    {
        static ImVec4 bull_col(0,1,0,1);
        static ImVec4 bear_col(1,0,0,1);

        ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(GetWindowSize(), ImGuiCond_Always);
        ImGui::Begin("##ImStocks", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize/*|ImGuiWindowFlags_MenuBar*/);

        // ImGuiIO &io = ImGui::GetIO();

        if(ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow), false))
        {
            timeline += 1;
        }
        if(ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow), false))
        {
            timeline -= 1;
        }

        static char buff[8] = "AAPL";
        ImGui::SetNextItemWidth(200);
        if (ImGui::InputText("##Ticker",buff,8,ImGuiInputTextFlags_EnterReturnsTrue|ImGuiInputTextFlags_CharsUppercase)) {

        }

        ImGui::SameLine();

        static int yrs = 2;
        ImGui::SetNextItemWidth(200);
        if (ImGui::Combo("##Years", &yrs, "1 Year\0""2 Years\0""5 Years\0""10 Years\0")) {
            if (yrs == 0)
                t1 = ImPlot::AddTime(t2, ImPlotTimeUnit_Yr, -1);
            else if (yrs == 1)
                t1 = ImPlot::AddTime(t2, ImPlotTimeUnit_Yr, -2);
            else if (yrs == 2)
                t1 = ImPlot::AddTime(t2, ImPlotTimeUnit_Yr, -5);
            else if (yrs == 3)
                t1 = ImPlot::AddTime(t2, ImPlotTimeUnit_Yr, -10);
        }

        ImGui::SameLine();

        if (ImGui::Button("Fetch")) {
            ImPlot::FormatDate(t1,t1_str,32,ImPlotDateFmt_DayMoYr,true);
            ImPlot::FormatDate(t2,t2_str,32,ImPlotDateFmt_DayMoYr,true);
            auto d = m_api.get_ticker(buff, t1_str, t2_str, Interval_Daily);
            if (d.ticker != "ERROR")
                m_ticker_data[d.ticker] = d;
            else
                fmt::print("Failed to get data for ticker symbol {}!\n",buff);
        }

        ImGui::SameLine();
        // ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);

        if (ImGui::BeginTabBar("TickerTabs")) {
            for (auto& pair : m_ticker_data) {
                auto& data = pair.second;
                if (ImGui::BeginTabItem(data.ticker.c_str())) {
                    static float ratios[] = {2,1};
                    if (ImPlot::BeginSubplots("##Stocks",2,1,ImVec2(-1,-1),ImPlotSubplotFlags_LinkCols,ratios)) {
                        if (ImPlot::BeginPlot("##OHLCPlot")) {
                            ImPlot::SetupAxes(0,0,ImPlotAxisFlags_NoTickLabels,ImPlotAxisFlags_AutoFit|ImPlotAxisFlags_RangeFit|ImPlotAxisFlags_Opposite);
                            ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
                            ImPlot::SetupAxisLimits(ImAxis_X1, data.time[0], data.time.back());
                            ImPlot::SetupAxisFormat(ImAxis_Y1, "$%.0f");
                            TickerTooltip(data, true);
                            ImPlot::SetNextFillStyle(ImVec4(0.5,0.5,1,1),0.25f);
                            ImPlot::PlotShaded("BB",data.time.data(),data.bollinger_top.data(),data.bollinger_bot.data(), std::min(timeline, data.size()));
                            ImPlot::SetNextLineStyle(ImVec4(0.5,0.5,1,1));
                            ImPlot::PlotLine("BB",data.time.data(),data.bollinger_mid.data(), std::min(timeline, data.size()));
                            PlotOHLC(timeline, "OHLC",data,bull_col,bear_col);
                            ImPlot::SetNextLineStyle(ImVec4(1,1,1,1));
                            ImPlot::PlotLine("Close",data.time.data(),data.close.data(), std::min(timeline, data.size()));
                            ImPlotRect bnds = ImPlot::GetPlotLimits();
                            int close_idx = BinarySearch(data.time.data(), 0, data.size() - 1, ImPlot::RoundTime(ImPlotTime::FromDouble(bnds.X.Max), ImPlotTimeUnit_Day).ToDouble());
                            if (close_idx == -1)
                                close_idx = data.time.size()-1;
                            double close_val = data.close[close_idx];
                            ImPlot::TagY(close_val, data.open[close_idx] < data.close[close_idx] ? bull_col : bear_col);
                            ImPlot::EndPlot();
                        }
                        if (ImPlot::BeginPlot("##VolumePlot")) {
                            ImPlot::SetupAxes(0,0,0,ImPlotAxisFlags_AutoFit|ImPlotAxisFlags_RangeFit|ImPlotAxisFlags_Opposite);
                            ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
                            ImPlot::SetupAxisLimits(ImAxis_X1, data.time[0], data.time.back());
                            ImPlot::SetupAxisFormat(ImAxis_Y1, VolumeFormatter);
                            TickerTooltip(data, true);
                            ImPlot::SetNextFillStyle(ImVec4(1.f,0.75f,0.25f,1));
                            ImPlot::PlotBars("Volume",data.time.data(),data.volume.data(),std::min(timeline, data.size()),60*60*24*0.5);
                            ImPlot::EndPlot();
                        }
                        ImPlot::EndSubplots();
                    }
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    YahooFinanceAPI   m_api;
    std::map<std::string,TickerData> m_ticker_data;
};

#ifdef _DEBUG
int main(int argc, char const *argv[])
#else
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#endif
{
    /*
    zmq::context_t ctx(0);

    auto thread1 = std::async(std::launch::async, PublisherThread, &ctx);

    // Give the publisher a chance to bind, since inproc requires it
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto thread2 = std::async(std::launch::async, SubscriberThread1, &ctx);
    auto thread3 = std::async(std::launch::async, SubscriberThread2, &ctx);
    */

    // system("python -c \"import yfinance as yf; df = yf.download('AAPL'); df.to_csv('AAPL.csv')\"");
#ifdef _DEBUG
    ImStocks app("ImStocks",960,540, argc, argv);
#else
    ImStocks app("ImStocks",960,540,(int)__argc, (char const**)__argv);
#endif
    app.Run();

    return 0;
}


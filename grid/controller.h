#ifndef GRID_CONTROLLER_H_
#define GRID_CONTROLLER_H_

#include <cairomm/context.h>
#include <cairomm/refptr.h>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "message_box.h"
#include "object.h"
#include "single_message_box.h"
#include "stream_reader.h"

namespace Grid {

class Board;
class Options;
class Painter;
class Viewer;

int MakeColor(int r, int g, int b);

double GetDoubleR(int color);
double GetDoubleG(int color);
double GetDoubleB(int color);

class Controller {
 public:
  Controller();
  virtual ~Controller() = default;

  // You -> Board.
  // -------------

  void Clear();

  void SetFieldColor(int x, int y, int r, int g, int b);
  void SetObject(int x, int y, Object object, int r, int g, int b);
  StreamReader SetText(int x, int y);
  void SetFog();

  void CenterOn(int x, int y);

  StreamReader AddMessage();

  void AddSingleMessageBox(double r, double g, double b, double a,
                           std::function<void(StreamReader&)> generator);

  void OnFieldClick(std::function<void(int x, int y, int button)> callback);
  void OnKeyPress(std::function<void(const std::string&)> callback);


  // Board -> Controller -> Board.
  // ----------------------

  void GetExtensions(int& min_x, int& min_y, int& max_x, int& max_y);

  void GetFieldInfo(int x, int y, bool& border, int& background, int& object,
                    std::string& text, bool& fog);

  void FieldClick(int x, int y, int button);
  void KeyPress(const std::string& key);

 private:
  friend class Viewer;
  friend int RunBoard(int argc, char** argv,
                      const Options& options, std::unique_ptr<Board> board,
                      std::function<void()> user_thread);

  void Draw(double width, double height,
            const Cairo::RefPtr<Cairo::Context>& context);

  bool IsInitialized() const;

  void SetOptions(const Options* options);
  void SetViewer(Viewer* viewer);
  void SetPainter(Painter* painter);

  const Options& options();
  Viewer& viewer();
  Painter& painter();

  // Guards the whole object.
  std::mutex mutex_;

  const Options* options_;
  Viewer* viewer_;
  Painter* painter_;

  MessageBox main_message_box_;
  std::vector<SingleMessageBox> single_message_boxes_;

  std::function<void(int, int, int)> on_field_click_callback_;
  std::function<void(const std::string&)> on_key_press_callback_;

  void InvalidateField(int x, int y);
  void InvalidateEverything();

  struct Field {
    int background;
    int object;
    std::string text;
    int64_t last_update_time;
  };

  int64_t current_time_;

  // Requires a lock.
  Field* GetField(int x, int y, bool force);

  int min_x_, max_x_, min_y_, max_y_;
  std::map<std::pair<int, int>, Field> fields_;
};

}  // namespace Grid

#endif  // GRID_CONTROLLER_H_

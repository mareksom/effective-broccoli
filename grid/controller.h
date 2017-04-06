#ifndef GRID_CONTROLLER_H_
#define GRID_CONTROLLER_H_

#include <cairomm/context.h>
#include <cairomm/refptr.h>
#include <functional>
#include <memory>
#include <string>

#include "message_box.h"
#include "options.h"
#include "single_message_box.h"
#include "stream_reader.h"

namespace Grid {

class Board;
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

  void InvalidateField(int x, int y);
  void CenterOn(int x, int y);

  StreamReader AddMessage();

  void AddSingleMessageBox(double r, double g, double b, double a,
                           std::function<void(StreamReader&)> generator);


  // Board -> You -> Board.
  // ----------------------

  virtual int BoardWidth() = 0;
  virtual int BoardHeight() = 0;

  virtual int GetFieldColor(int x, int y) = 0;
  virtual int GetObject(int x, int y) = 0;


  // Board -> You.
  // -------------

  // A button was clicked:
  //  1 - left
  //  2 - middle
  //  3 - right
  // If there are more buttons, it is possible that @button will not belong
  // to the set {1, 2, 3}.
  virtual void FieldClick(int x, int y, int button) = 0;

  virtual void KeyPress(const std::string& key) = 0;

 private:
  friend class Viewer;
  friend int RunBoard(int argc, char** argv,
                      Options options, std::unique_ptr<Board> board);

  void Draw(double width, double height,
            const Cairo::RefPtr<Cairo::Context>& context);

  bool IsInitialized() const;

  void SetOptions(const Options* options);
  void SetViewer(Viewer* viewer);
  void SetPainter(Painter* painter);

  const Options& options();
  Viewer& viewer();
  Painter& painter();

  const Options* options_;
  Viewer* viewer_;
  Painter* painter_;

  MessageBox main_message_box_;
  std::vector<SingleMessageBox> single_message_boxes_;
};

}  // namespace Grid

#endif  // GRID_CONTROLLER_H_

#ifndef OPTIONS_H_
#define OPTIONS_H_

class Controller;

class Options {
 public:
  // Contains default options.
  Options();

  Controller* controller() const;
  void SetController(Controller* controller);

  bool MaximizeOnStart() const;
  void SetMaximizeOnStart(bool maximize);

  int WindowWidthOnStart() const;
  int WindowHeightOnStart() const;
  void SetWindowSizeOnStart(int width, int height);

  int NumberOfFieldsProcessedPerFrame() const;
  void SetNumberOfFieldsProcessedPerFrame(int number_of_fields);

  double InitialScale() const;
  void SetInitialScale(double scale);

  double ScrollSpeed() const;
  void SetScrollSpeed(double speed);

  double ZoomSpeed() const;
  void SetZoomSpeed(double speed);

  // The color is a gray-scale color - a number in range [0, 255].
  // This color indicates fields that haven't been drawn yet.
  int NullColor() const;
  void SetNullColor(int color);

  double MessageBoxesMargin() const;
  void SetMessageBoxesMargin(double margin);

  double MainMessageBoxMaxWidth() const;
  void SetMainMessageBoxMaxWidth(double width);

  double SingleBoxMessageHeight() const;
  void SetSingleBoxMessageHeight(double height);

 private:
  Controller* controller_ = nullptr;

  bool maximize_on_start_ = false;
  int window_width_on_start_ = 800;
  int window_height_on_start_ = 600;

  int number_of_fields_processed_per_frame_ = 1000;

  double initial_scale_ = 100.0;

  double scroll_speed_ = 15.0;
  double zoom_speed_ = 1.2;

  int null_color_ = 200;

  double message_boxes_margin_ = 10;
  double main_message_box_max_width_ = 300;
  double single_box_message_height_ = 25;
};

#endif  // OPTIONS_H_

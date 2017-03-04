#ifndef OPTIONS_H_
#define OPTIONS_H_

class Options {
 public:
  // Contains default options.
  Options();

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

 private:
  bool maximize_on_start_ = false;
  int window_width_on_start_ = 800;
  int window_height_on_start_ = 600;

  int number_of_fields_processed_per_frame_ = 1000;

  double initial_scale_ = 100.0;

  double scroll_speed_ = 15.0;
  double zoom_speed_ = 1.2;
};

extern Options options;

#endif  // OPTIONS_H_

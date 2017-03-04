#ifndef OPTIONS_H_
#define OPTIONS_H_

class Options {
 public:
  // Contains default options.
  Options();

  double ScrollSpeed() const;
  void SetScrollSpeed(double speed);

  double ZoomSpeed() const;
  void SetZoomSpeed(double speed);

 private:
  double scroll_speed_ = 15.0;
  double zoom_speed_ = 1.2;
};

extern Options options;

#endif  // OPTIONS_H_

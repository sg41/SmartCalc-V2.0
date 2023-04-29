#ifndef CONSOLEVIEW_H
#define CONSOLEVIEW_H
#include <iostream>

#include "../controllers/exampleController.h"
#include "../models/graphModel.h"

enum Choice { VAR = 1, FORMULA = 2, CALC = 3, EXIT = 0, NONE = -1 };

class ConsoleView : public ModelObserverInterface<GraphModelData> {
 private:
  ExampleController *controller = nullptr;
  GraphModelData *data = nullptr;
  // void (*user_action)(void);

 public:
  ConsoleView(ExampleController *c) : controller(c) {
    // ConsoleView() {
    data = new GraphModelData;
    data->dx = (double)(data->MAXX - data->MINX) / data->MAXI;
    data->dy = (double)data->MAXJ / (data->MAXY - data->MINY);
  };
  ~ConsoleView() { delete data; };
  GraphModelData *get_data() { return data; }
  void displayMenu();
  int performChoice();
  double performNumericInput();
  std::string performFormulaInput();
  void startEventLoop();
  void draw_txt_graph();
  void update(const GraphModelData *model_data) override {
    *data = *(model_data);
    std::cout << "Current result for X=" << data->x << " is: " << data->y
              << std::endl;
    draw_txt_graph();
  };
};

#endif
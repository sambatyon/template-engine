#pragma once

struct RenderTests {
  int RunTests();

  int TestFlatTemplate();
  int TestSimpleTemplate();
  int TestTemplateWithLoop();
  int TestTemplateNestedLoop();
  int TestTemplateVariableShadowing();
  int TestRenderErrors();
  int TestLoopWithEmptyArray();
};

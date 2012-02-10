#include "test-helpers.hh"

////////////////////////////////////////////////////////////////////////

int drawingAreaWidth=0;
int drawingAreaHeight=0;

TestData::Ptr testData;

////////////////////////////////////////////////////////////////////////

TestData::TestData(TiffPresentation::Ptr tp, const LayerSpec& ls,
                   TiledBitmapInterface::Ptr tbi, SourcePresentation* sp, int zoom)
  : pi(new ProgressInterfaceStub()), vi(new ViewInterfaceStub(pi)), tp(tp), ls(ls), tbi(tbi), sp(sp), zoom(zoom)
{
  tbi->open(vi);
}

TestData::Ptr TestData::create(TiffPresentation::Ptr tp, const LayerSpec& ls,
                               TiledBitmapInterface::Ptr tbi, SourcePresentation* sp, int zoom)
{
  return TestData::Ptr(new TestData(tp, ls, tbi, sp, zoom));
}

bool TestData::wait()
{
  return !pi->isFinished();
}

TestData::~TestData()
{
  tbi->close(vi);
  tbi.reset();
  delete sp;
  while(!ls.empty())
  {
    delete ls.back();
    ls.pop_back();
  }
  tp.reset();
  delete vi;
  delete pi;
}

void TestData::redraw(cairo_t* cr)
{
  if(tbi)
  {
    GdkRectangle rect;
    rect.x=0;
    rect.y=0;
    if(zoom>=0)
    {
      // Zooming in. Smallest step is 1 presentation pixel, which is more than one window-pixel
      int pixelSize = 1<<zoom;
      rect.width = (drawingAreaWidth+pixelSize-1)/pixelSize;
      rect.height = (drawingAreaHeight+pixelSize-1)/pixelSize;
    }
    else
    {
      // Zooming out. Smallest step is 1 window-pixel, which is more than one presentation-pixel
      int pixelSize = 1<<(-zoom);
      rect.width = drawingAreaWidth*pixelSize;
      rect.height = drawingAreaHeight*pixelSize;
    }
    
    tbi->redraw(vi, cr, rect, zoom);
  }
}

////////////////////////////////////////////////////////////////////////

Sleep::Sleep(unsigned int secs)
  : secs(secs), started(false)
{
}

bool Sleep::operator()()
{
  if(!started && 0==clock_gettime(CLOCK_REALTIME, &t))
  {
    started = true;
    return true;
  }

  struct timespec now;
  if(0==clock_gettime(CLOCK_REALTIME, &now))
  {
    if(now.tv_sec > t.tv_sec + secs)
      return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////////

bool quit()
{
  gtk_main_quit();

  return false;
}

bool reset()
{
  testData.reset();

  return false;
}

bool wait()
{
  if(testData)
    return testData->wait();
  else
    return false;
}

////////////////////////////////////////////////////////////////////////

bool setupTest1bpp(int zoom, int width, int height)
{
  TiffPresentation::Ptr tp(new TiffPresentation());
  Colormap::Ptr colormap = Colormap::createDefault(2);
  tp->setColormap(colormap);

  LayerSpec ls;
  ls.push_back(new Operations1bpp(tp.get()));
  ls.push_back(new Operations8bpp(tp.get()));

  TiledBitmapInterface::Ptr tbi = createTiledBitmap(width, height, ls);
  SourcePresentation* sp = new Source1Bpp();
  tbi->setSource(sp);

  testData = TestData::create(tp, ls, tbi, sp, zoom);

  return false;
}

bool setupTest2bpp(int zoom, int width, int height)
{
  TiffPresentation::Ptr tp(new TiffPresentation());
  Colormap::Ptr colormap = Colormap::createDefault(16);
  tp->setColormap(colormap);

  LayerSpec ls;
  ls.push_back(new Operations(tp.get(), 2));
  ls.push_back(new OperationsColormapped(tp.get(), 2));

  TiledBitmapInterface::Ptr tbi = createTiledBitmap(width, height, ls);
  SourcePresentation* sp = new Source2Bpp();
  tbi->setSource(sp);

  testData = TestData::create(tp, ls, tbi, sp, zoom);

  return false;
}

bool setupTest4bpp(int zoom, int width, int height)
{
  TiffPresentation::Ptr tp(new TiffPresentation());
  Colormap::Ptr colormap = Colormap::createDefault(16);
  tp->setColormap(colormap);

  LayerSpec ls;
  ls.push_back(new Operations(tp.get(), 4));
  ls.push_back(new OperationsColormapped(tp.get(), 4));

  TiledBitmapInterface::Ptr tbi = createTiledBitmap(width, height, ls);
  SourcePresentation* sp = new Source4Bpp();
  tbi->setSource(sp);

  testData = TestData::create(tp, ls, tbi, sp, zoom);

  return false;
}

bool setupTest8bpp(int zoom, int width, int height)
{
  TiffPresentation::Ptr tp(new TiffPresentation());
  Colormap::Ptr colormap = Colormap::createDefault(2);
  tp->setColormap(colormap);

  LayerSpec ls;
  ls.push_back(new Operations8bpp(tp.get()));

  TiledBitmapInterface::Ptr tbi = createTiledBitmap(width, height, ls);
  SourcePresentation* sp = new Source8Bpp();
  tbi->setSource(sp);

  testData = TestData::create(tp, ls, tbi, sp, zoom);

  return false;
}

bool setupTest8bppColormapped(int zoom, int width, int height)
{
  TiffPresentation::Ptr tp(new TiffPresentation());
  Colormap::Ptr colormap = Colormap::createDefault(256);
  tp->setColormap(colormap);

  LayerSpec ls;
  ls.push_back(new Operations(tp.get(), 8));
  ls.push_back(new OperationsColormapped(tp.get(), 8));

  TiledBitmapInterface::Ptr tbi = createTiledBitmap(width, height, ls);
  SourcePresentation* sp = new Source8Bpp();
  tbi->setSource(sp);

  testData = TestData::create(tp, ls, tbi, sp, zoom);

  return false;
}
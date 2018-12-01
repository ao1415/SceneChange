#include <Siv3D.hpp>
#include <HamFramework.hpp>

//切り替えエフェクト用のEffectクラスを共有で持つ
struct GameData {
	Effect fadeEffect;
};

using MyApp = SceneManager<String, GameData>;

//1キーに割り当てたエフェクト
struct BoxEffect : IEffect {

	//フェードイン/アウト含めた切り替え時間
	double transitionTime;
	//各箇所でボックスが発生する時間
	Grid<double> boxRadius;

	BoxEffect(const MillisecondsF& transitionTime) {
		this->transitionTime = static_cast<int32>(transitionTime.count()) / 1000.0;

		const Size size(4, 4);
		boxRadius.resize(size, 0);
		const double half = this->transitionTime / 2; //フェードイン・アウトの時間
		const double limit = half / 2; //一か所の変更にかける時間
		const double time = half - limit; //すべての場所を変化させ始める時間

		for (const auto& p : step(size))
		{
			//左上から右下に順に変化するようにする
			boxRadius[p] = -(p.x + p.y) * time / (size.x + size.y);
		}

	}

	bool update(double t) override {

		if (t >= transitionTime)
		{
			return false;
		}

		const double half = transitionTime / 2;
		const double limit = half / 2;

		const Size& size = boxRadius.size();
		const Size boxSize = Window::ClientRect().size / size;
		if (t <= half)
		{
			//フェードイン
			for (const auto& p : step(size))
			{
				const Point pos = boxSize * p;

				//0～Pi/2に丸める
				const double rad = Clamp(boxRadius[p] + t, 0.0, limit) / limit * Math::HalfPi;

				const int w = static_cast<int>(Sin(rad) * boxSize.x);
				const int h = static_cast<int>(Sin(rad) * boxSize.y);

				Rect rect(pos.x, pos.y, w, h);
				rect.draw(Palette::Black);
			}
		}
		else
		{
			//フェードアウト
			for (const auto& p : step(size))
			{
				const Point pos = boxSize * p;

				const double rad = Clamp(boxRadius[p] + t - half, 0.0, limit) / limit * Math::HalfPi;
				const int w = static_cast<int>(Sin(rad) * boxSize.x);
				const int h = static_cast<int>(Sin(rad) * boxSize.y);

				if (rad < Math::HalfPi)
				{
					Rect rect(pos.x + w, pos.y + h, boxSize.x - w + 1, boxSize.y - h + 1);
					rect.draw(Palette::Black);
				}
			}
		}

		return true;
	}
};

//2キーに割り当てたエフェクト
struct CircleEffect : IEffect {

	//フェードイン/アウト含めた切り替え時間
	double transitionTime;

	CircleEffect(const MillisecondsF& transitionTime) {
		this->transitionTime = static_cast<int32>(transitionTime.count()) / 1000.0;
	}

	bool update(double t) override {

		if (t >= transitionTime)
		{
			return false;
		}

		const double half = transitionTime / 2;

		//円を配置する間隔
		const Size size(100, 100);
		//ぴったりだと円で塗りつぶされた感がないのでちょっと大きくする
		const double R = size.length() / 2 * 1.1;

		//一回り大きくする
		const Size grid = Window::ClientRect().size / size + Size(2, 2);

		if (t <= half)
		{
			//フェードイン
			for (const auto& p : step(grid))
			{
				const Point pos = p * size;

				const double rad = (t - 0) / half * Math::HalfPi;
				const int r = static_cast<int>(Sin(rad) * R);

				Circle(pos, r).draw(Palette::Black);
			}
		}
		else
		{
			//フェードアウト
			for (const auto& p : step(grid))
			{
				const Point pos = p * size;

				const double rad = (t - half) / half * Math::HalfPi;
				const int r = static_cast<int>(R - Sin(rad) * R);

				Circle(pos, r).draw(Palette::Black);
			}
		}

		return true;
	}
};

//3キーに割り当てたエフェクト
struct LineEffect : IEffect {

	//フェードイン/アウト含めた切り替え時間
	double transitionTime;

	//切り替え用の線の初期位置
	Array<std::pair<Point, Point>> lines;

	LineEffect(const MillisecondsF& transitionTime) {
		this->transitionTime = static_cast<int32>(transitionTime.count()) / 1000.0;

		const Size size = Window::ClientRect().size;
		lines.resize(size.y);

		for (int i = 0; i < size.y; i++)
		{
			//1/3で横幅*2が左に移動する
			lines[i].first.x = size.x + Random(size.x / 2, size.x);
			lines[i].first.y = i;

			//2/3で横幅*4を左に移動させる
			lines[i].second.x = lines[i].first.x + size.x * 2 * 2;
			lines[i].second.y = i;
		}

	}

	bool update(double t) override {

		if (t >= transitionTime)
		{
			return false;
		}

		const Size size = Window::ClientRect().size;
		const double half = transitionTime / 2;
		const double limit = transitionTime / 3;
		const double speed = size.x * 2 / limit;

		//フェードイン
		//フェードアウト
		for (const auto& line : lines)
		{
			const Point pos(static_cast<int>(speed*t), 0);

			Line(line.first - pos, line.second - pos).draw();
		}

		return true;
	}
};

//4キーに割り当てたエフェクト
struct CutEffect : IEffect {

	//フェードイン/アウト含めた切り替え時間
	double transitionTime;

	//フェードアウト用のフェードイン画像
	DynamicTexture fadeImage;

	CutEffect(const MillisecondsF& transitionTime) {
		this->transitionTime = static_cast<int32>(transitionTime.count()) / 1000.0;
	}

	bool update(double t) override {

		if (t >= transitionTime)
		{
			return false;
		}

		const double half = transitionTime / 2;

		const Size size(4, 4);
		const Size windowSize = Window::ClientRect().size;

		if (t <= half)
		{
			//フェードイン

			if (fadeImage.isEmpty())
			{
				if (ScreenCapture::HasNewFrame())
				{
					fadeImage = DynamicTexture(ScreenCapture::GetFrame());
				}
				ScreenCapture::RequestCurrentFrame();
			}
			else
			{
				for (int i = 1; i < size.x; i++)
				{
					const Point start = Point(0, windowSize.y * i / size.y);

					const double rad = (t - 0) / half * Math::HalfPi;
					const int w = static_cast<int>(Sin(rad) * windowSize.x);

					const Point end = Point(w, start.y);

					Line(start, end).draw(1, Palette::White);
				}
				for (int i = 1; i < size.x; i++)
				{
					const Point start = Point(windowSize.x * i / size.x, 0);

					const double rad = (t - 0) / half * Math::HalfPi;
					const int h = static_cast<int>(Sin(rad) * windowSize.y);

					const Point end = Point(start.x, h);

					Line(start, end).draw(1, Palette::White);
				}
			}
		}
		else
		{
			//フェードアウト

			for (const auto& p : step(size))
			{
				const Point pos = windowSize * p / size;
				const Point nextPos = windowSize * (p + Size(1, 1)) / size;
				const Size rectSize = nextPos - pos;
				Rect drawRect(pos, rectSize);

				const double rad = (t - half) / half * Math::HalfPi;
				const double s = 1 - Sin(rad);


				fadeImage(drawRect).scaled(s).drawAt(drawRect.center()).drawFrame(1, Palette::White);
			}
		}

		return true;
	}
};

struct Scene01 : public MyApp::Scene {

	Scene01(const InitData& init) : IScene(init) {

		auto& sceneEffect = getData().fadeEffect;

		//1～4キーにシーン切り替えとエフェクトを割り当てる
		selectScene.push_back({ Key1, [&]() {
			sceneEffect.add<BoxEffect>(2s);
			changeScene(U"Scene02",2s);
		} });
		selectScene.push_back({ Key2, [&]() {
			sceneEffect.add<CircleEffect>(2s);
			changeScene(U"Scene02",2s);
		} });
		selectScene.push_back({ Key3, [&]() {
			sceneEffect.add<LineEffect>(2s);
			changeScene(U"Scene02",2s);
		} });
		selectScene.push_back({ Key4, [&]() {
			sceneEffect.add<CutEffect>(2s);
			changeScene(U"Scene02",2s);
		} });

	}

	//フェードイン/アウトを上書きしてさせないようにする
	void drawFadeIn(double) const override { draw(); }
	void drawFadeOut(double) const override { draw(); }

	void update() override {

		for (const auto& scene : selectScene)
		{
			if (scene.first.down())
			{
				scene.second();
				break;
			}
		}

		r += 2_deg;
	}

	void draw() const override {

		Rect(240, 120, 150).rotated(r).draw(Palette::Yellowgreen);
		Rect(480, 400, 300).rotated(r + 45_deg).draw(Palette::Yellowgreen);

	}

	//描画用変数
	double r = 0;

	//切り替え方法を格納する
	Array<std::pair<Key, std::function<void(void)>>> selectScene;

};

struct Scene02 : public MyApp::Scene {

	Scene02(const InitData& init) : IScene(init) {

		auto& sceneEffect = getData().fadeEffect;

		//1～4キーにシーン切り替えとエフェクトを割り当てる
		selectScene.push_back({ Key1, [&]() {
			sceneEffect.add<BoxEffect>(2s);
			changeScene(U"Scene01",2s);
		} });
		selectScene.push_back({ Key2, [&]() {
			sceneEffect.add<CircleEffect>(2s);
			changeScene(U"Scene01",2s);
		} });
		selectScene.push_back({ Key3, [&]() {
			sceneEffect.add<LineEffect>(2s);
			changeScene(U"Scene01",2s);
		} });
		selectScene.push_back({ Key4, [&]() {
			sceneEffect.add<CutEffect>(2s);
			changeScene(U"Scene01",2s);
		} });

	}

	//フェードイン/アウトを上書きしてさせないようにする
	void drawFadeIn(double) const override { draw(); }
	void drawFadeOut(double) const override { draw(); }

	void update() override {

		for (const auto& scene : selectScene)
		{
			if (scene.first.down())
			{
				scene.second();
				break;
			}
		}

		r += 4_deg;
	}

	void draw() const override {

		Circle(400 + (Clamp(Sin(r), -0.8, 0.8) / 0.8) * 200, 120, 60).draw(Palette::Green);
		Circle(400 + (Clamp(Sin(r + 22.5_deg), -0.1, 0.8) / 0.8) * 200, 400, 80).draw(Palette::Green);

	}

	//描画用変数
	double r = 0;

	//切り替え方法を格納する
	Array<std::pair<Key, std::function<void(void)>>> selectScene;

};

void Main() {

	Graphics::SetBackground(Palette::Lightblue);

	auto p = MakeShared<GameData>();

	MyApp manager(p);
	manager.add<Scene01>(U"Scene01");
	manager.add<Scene02>(U"Scene02");
	manager.changeScene(U"Scene01", 0, false);

	while (System::Update())
	{
		//シーンを動かす
		if (!manager.update())
		{
			break;
		}
		//切り替え用のエフェクトを動かす
		p->fadeEffect.update();
	}
}

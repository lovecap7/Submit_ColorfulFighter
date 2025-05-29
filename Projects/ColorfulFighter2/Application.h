#pragma once

/// <summary>
/// シングルトン
/// </summary>
class Application
{
private:
	Application() {}
	//関数宣言の後に = deleteでその関数の使用を禁止できる
	Application(const Application&) {}
	void operator=(const Application&) {}
public:
	/// <summary>
	/// シングルトンオブジェクトを返す関数
	/// 参照を返すことで、複製を防ぐ
	/// 間違えて値を返した場合、コピーコンストラクタが走るので
	/// それをprivateにしておくことで弾くことができる
	/// </summary>
	/// <returns></returns>
	static Application& GetInstance();

	/// <summary>
	/// アプリケーションの初期化
	/// </summary>
	/// <returns>ture:初期化成功, false:失敗</returns>
	bool Init();

	void Run();

	void Terminate();
};

//const
//値の変更ができない
//関数の後ろについている場合はconstの持ち主の状態が変更不可

//static
//アドレスを確定させる
//どこからでもアクセスできる(グローバル)
//ローカル変数がstaticの場合はそのローカル変数の持ち主しかアクセスできない
//シングルトンオブジェクトはただ一つだけであることを保証されているためアクセスによって
//別のオブジェクトを見ているというようなことは起きない

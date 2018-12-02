# SceneChange
__このリポジトリは[Siv3D AdventCalendar 2018](https://qiita.com/advent-calendar/2018/siv3d)用に作成されました。__

シーン切り替えをエフェクトで行うサンプルです

作成環境
- Visual Studio 2017
- OpenSiv3D(0.3.1)

## プログラム説明
__BoxEffect__  
__CircleEffect__  
__Lineffect__  
__CutEffect__  
シーン間で動作するエフェクトです  
SceneChangeと同時に生成することで切り替えのアニメーションを行います  

__Scene01__  
__Scene02__  
適当なアニメーションを流すシーンです  
1,2,3,4キーを押下することでScene01とScene02を遷移します  
それぞれのキーは以下のような対応になっています  
- 1：BoxEffect
- 2：CircleEffect
- 3：Lineffect
- 4：CutEffect

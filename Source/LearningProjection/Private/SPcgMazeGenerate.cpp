// Fill out your copyright notice in the Description page of Project Settings.


#include "SPcgMazeGenerate.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASPcgMazeGenerate::ASPcgMazeGenerate()
{
	MeshFloor = CreateDefaultSubobject<UStaticMeshComponent>("MeshFloor");
	RootComponent = MeshFloor;
}

// Called when the game starts or when spawned
void ASPcgMazeGenerate::BeginPlay()
{
	Super::BeginPlay();

	FVector Scalar = UKismetMathLibrary::MakeVector(rows, cols, 1.0f);
	FTransform Transform = UKismetMathLibrary::MakeTransform(GetActorLocation(), GetActorRotation(), Scalar);

	MeshFloor->SetWorldTransform(Transform);	//����rows��cols�����õذ�����

	//��ʼ��TArray
	for (int i = 0; i < rows; ++i)
	{
		MazeTypes.SetNum(rows);
		MazeDoors.SetNum(rows);
		MazeTypes[i].SetNum(cols);
		MazeDoors[i].SetNum(cols);
		for (int j = 0; j < cols; ++j)
		{
			MazeTypes[i][j] = i * cols + j;
			MazeDoors[i][j].SetNum(4);
			for (int k = 0; k < 4; ++k)
			{
				MazeDoors[i][j][k] = false;
			}
		}
	}

	GenerateMaze();
}

void ASPcgMazeGenerate::GenerateMaze()
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			TArray<int> candidateDoorDir;	//0�ϣ�1�ң�2�£�3��
			//�Ƿ������Ͽ���
			if (i > 0 && MazeTypes[i][j] != MazeTypes[i - 1][j]) {
				candidateDoorDir.Add(0);
			}
			//�Ƿ������ҿ���
			if (j < cols - 1 && MazeTypes[i][j] != MazeTypes[i][j + 1]) {
				candidateDoorDir.Add(1);
			}
			//�Ƿ������¿���
			if (i < rows - 1 && MazeTypes[i][j] != MazeTypes[i + 1][j]) {
				candidateDoorDir.Add(2);
			}
			//�Ƿ���������
			if (j > 0 && MazeTypes[i][j] != MazeTypes[i][j - 1]) {
				candidateDoorDir.Add(3);
			}

			if (candidateDoorDir.Num() == 0) {
				break;
			}

			//�ڿ��Կ��ŵı�ѡ���������ѡһ��������
			int openDoorDir = candidateDoorDir[std::rand() % candidateDoorDir.Num()];
			int newRoomID = -1;
			//����
			MazeDoors[i][j][openDoorDir] = true; 
			if (openDoorDir == 0) {
				MazeDoors[i - 1][j][2] = true;
				newRoomID = MazeTypes[i - 1][j];
			}
			else if (openDoorDir == 1) {
				MazeDoors[i][j + 1][3] = true; 
				newRoomID = MazeTypes[i][j + 1];
			}
			else if (openDoorDir == 2) {
				MazeDoors[i + 1][j][0] = true;
				newRoomID = MazeTypes[i + 1][j];
			}
			else if (openDoorDir == 3) {
				MazeDoors[i][j - 1][1] = true;
				newRoomID = MazeTypes[i][j - 1];
			}

			//�������뵱ǰ��������ͬ�ķ��䶼��Ϊ�µķ����,�˴����ò��鼯��һ���Ż��㷨
			int oldRoomID = MazeTypes[i][j];
			for (int ii = 0; ii < rows; ii++)
			{
				for (int jj = 0; jj < cols; jj++) 
				{
					if (MazeTypes[ii][jj] == oldRoomID) {
						MazeTypes[ii][jj] = newRoomID;
					}
				}
			}
		}
	}



	//����MazeDoors������Ӧ��Wall

	FTransform SpawnTM;		//����Wall���ɾ���

	FActorSpawnParameters SpawnParameters;	//����SpawnActor()���������ɲ���
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;//����SpawnActor()���������ɲ���

	for (int i_wall = 0; i_wall < rows; i_wall++)
	{
		for (int j_wall = 0; j_wall < cols; j_wall++)
		{
			//�����ϱߵ�ǽ��
			if (!MazeDoors[i_wall][j_wall][0])
			{
				float xPos = j_wall * RoomExtent + GetActorLocation().X;
				float yPos = i_wall * RoomExtent + GetActorLocation().Y;

				SpawnTM = FTransform(FRotator::ZeroRotator, FVector(FIntPoint(xPos, yPos)));
				GetWorld()->SpawnActor<AActor>(MeshWall, SpawnTM, SpawnParameters);
			}
			//������ߵ�ǽ��
			if (!MazeDoors[i_wall][j_wall][3])
			{
				float xPos = j_wall * RoomExtent + GetActorLocation().X;
				float yPos = i_wall * RoomExtent + GetActorLocation().Y;

				SpawnTM = FTransform(FRotator(0, 90, 0), FVector(FIntPoint(xPos, yPos)));
				GetWorld()->SpawnActor<AActor>(MeshWall, SpawnTM, SpawnParameters);
			}
			//��������ҵķ���, �����ұߵ�ǽ��
			if (j_wall == cols - 1)
			{
				float xPos = cols * RoomExtent + GetActorLocation().X;
				float yPos = i_wall * RoomExtent + GetActorLocation().Y;

				SpawnTM = FTransform(FRotator(0, 90, 0), FVector(FIntPoint(xPos, yPos)));
				GetWorld()->SpawnActor<AActor>(MeshWall, SpawnTM, SpawnParameters);
			}
			//�����������ķ���,�����±ߵ�ǽ��
			if (i_wall == rows - 1)
			{
				float xPos = j_wall * RoomExtent + GetActorLocation().X;
				float yPos = rows * RoomExtent + GetActorLocation().Y;

				SpawnTM = FTransform(FRotator::ZeroRotator, FVector(FIntPoint(xPos, yPos)));
				GetWorld()->SpawnActor<AActor>(MeshWall, SpawnTM, SpawnParameters);
			}
		}
	}

}

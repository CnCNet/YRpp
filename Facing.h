#pragma once

#include <Dir.h>
#include <Timer.h>

class FacingClass
{
public:
	explicit FacingClass() noexcept { }
	explicit FacingClass(int rate) noexcept
	{
		Set_ROT(rate);
	}
	explicit FacingClass(const DirStruct& facing) noexcept
	{
		DesiredFacing = facing;
	}
	explicit FacingClass(DirType dir) noexcept
	{
		DesiredFacing.Set_Dir(dir);
	}
	explicit FacingClass(const FacingClass& another) noexcept
		: DesiredFacing { another.DesiredFacing }
		, StartFacing { another.StartFacing }
		, RotationTimer { another.RotationTimer }
		, ROT { another.ROT }
	{ }

	FacingClass& operator=(const FacingClass& another)
	{
		if (this != &another)
		{
			DesiredFacing = another.DesiredFacing;
			StartFacing = another.StartFacing;
			RotationTimer = another.RotationTimer;
			ROT = another.ROT;
		}

		return *this;
	}

	bool Set_Desired(const DirStruct& facing)
	{
		if (DesiredFacing == facing)
			return false;

		StartFacing = Current();
		DesiredFacing = facing;

		if (ROT.Raw > 0)
			RotationTimer.Start(NumSteps());

		return true;
	}
	bool Set_Current(const DirStruct& facing)
	{
		bool ret = Current() != facing;
		if (ret)
		{
			DesiredFacing = facing;
			StartFacing = facing;
		}
		RotationTimer.Start(0);
		return ret;
	}

	DirStruct Desired() const
	{
		return DesiredFacing;
	}
	DirStruct Current() const
	{
		if (Is_Rotating())
		{
			const short diff = Difference().Raw;
			const int num_steps = NumSteps();
			if (num_steps > 0)
			{
				const int steps_left = RotationTimer.GetTimeLeft();
				return DirStruct { DesiredFacing.Raw - steps_left * diff / num_steps };
			}
		}
		return DesiredFacing;
	}

	bool Is_Rotating() const
	{
		if (static_cast<short>(ROT.Raw) <= 0)
			return false;

		return RotationTimer.GetTimeLeft() != 0;
	}
	bool Is_Rotating_L() const
	{
		if (!Is_Rotating())
			return false;

		return Difference().Raw < 0;
	}
	bool Is_Rotating_G() const
	{
		if (!Is_Rotating())
			return false;

		return Difference().Raw > 0;
	}

	DirStruct Difference() const
	{
		return DirStruct { static_cast<int>(DesiredFacing.Raw - StartFacing.Raw) };
	}

	void Set_ROT(int rate)
	{
		if (rate > 127)
			rate = 127;
		ROT.Set_Dir(static_cast<DirType>(rate));
	}

private:
	int NumSteps() const
	{
		return std::abs(Difference().Raw) / ROT.Raw;
	}

public:

	DirStruct DesiredFacing;
	DirStruct StartFacing; // The starting direction from which to calcuate the rotation.
	CDTimerClass RotationTimer;
	DirStruct ROT;
};
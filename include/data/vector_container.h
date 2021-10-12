#pragma once
#include "../common/setup.h";
#include "../common/common.h";
#include "../core/template_concept.h";
#include "../core/template_core.h";
#include "../simd/simd_operation.h"


namespace vmath_vector_container {

	template<typename T, size_t Size>
	struct vectorContainer;

	template<size_t Size>
	constexpr bool is_vector_heap_alloc_v = Size > static_cast<size_t>(32);

	template<typename T, size_t Size>
		requires (Size > 0 && !is_vector_heap_alloc_v<Size>)
	struct vectorContainer<T, Size>
	{
		typedef T Scalar;
		Scalar m_Data[Size];

		vectorContainer() noexcept = default;

		vectorContainer(const vectorContainer& rhs)
		{
			std::memcpy(m_Data, rhs.data(), Size * sizeof(Scalar));
		}

		vectorContainer(const vectorContainer<Scalar, DYNAMIC_SIZE>& rhs)
		{
			std::memcpy(m_Data, rhs.data(), Size * sizeof(Scalar));
		}

		template<typename U>
			requires std::is_convertible_v<U, Scalar>
		vectorContainer(const vectorContainer<U, Size>& rhs)
		{
			size_t index = 0;
			for (; index < Size; index += vmath_template_core::unrollStride_fix<Size>)
			{
				vmath_template_core::fowardUnroll
					<vmath_template_core::unrollStride_fix<Size>>
				(
					[&](size_t i)
					{
						m_Data[index + i] = static_cast<Scalar>(rhs[index + i]);
					}
				);
			}
			if constexpr (Size % vmath_template_core::unrollStride_fix<Size>)
			{
				for (; index < Size; ++index)
				{
					m_Data[index] = static_cast<Scalar>(rhs[index]);
				}
			}
		}

		template<typename U>
		vectorContainer(const vectorContainer<U, DYNAMIC_SIZE>& rhs)
		{
			for (size_t index = 0; index < rhs.size(); ++index)
			{
				m_Data[index] = rhs[index];
			}
		}

		vectorContainer& operator=(const vectorContainer& rhs)
		{
			if (this != &rhs)
			{
				std::memcpy(m_Data, rhs.data(), Size * sizeof(Scalar));
			}
			return *this;
		}

		vectorContainer& operator=(const vectorContainer<Scalar, DYNAMIC_SIZE>& rhs)
		{
			std::memcpy(m_Data, rhs.data(), Size * sizeof(Scalar));
			return *this;
		}

		template<typename U>
			requires std::is_convertible_v<U, Scalar>
		vectorContainer& operator=(const vectorContainer<U, Size>& rhs)
		{
			size_t index = 0;
			for (; index < Size; index += vmath_template_core::unrollStride_fix<Size>)
			{
				vmath_template_core::fowardUnroll
					<vmath_template_core::unrollStride_fix<Size>>
				(
					[&](size_t i)
					{
						m_Data[index + i] = static_cast<Scalar>(rhs[index + i]);
					}
				);
			}
			if constexpr (Size % vmath_template_core::unrollStride_fix<Size>)
			{
				for (; index < Size; ++index)
				{
					m_Data[index] = static_cast<Scalar>(rhs[index]);
				}
			}

			return *this;
		}

		template<typename U>
			requires std::is_convertible_v<U, Scalar>
		vectorContainer& operator=(const vectorContainer<U, DYNAMIC_SIZE>& rhs)
		{
			for (size_t index = 0; index < rhs.size(); ++index)
			{
				m_Data[index] = static_cast<Scalar>(rhs[index]);
			}
		}

		/*
		
			Basic Functionality
		
		*/

		VMATH_CONSTEXPR size_t size() const
		{
			return Size;
		}

		VMATH_INLINE Scalar* data()
		{
			return m_Data;
		}

		VMATH_INLINE const Scalar* data() const
		{
			return m_Data;
		}

		VMATH_INLINE Scalar& operator[](size_t index)
		{
			return m_Data[index];
		}

		VMATH_INLINE const Scalar& operator[](size_t index) const
		{
			return m_Data[index];
		}
	};

	template<typename U, size_t _Size>
		requires is_vector_heap_alloc_v<_Size>
	struct vectorContainer<U, _Size>
	{
		typedef U Scalar;
		Scalar* m_Data = nullptr;

		vectorContainer() 
			: m_Data(static_cast<Scalar*>(malloc(_Size * sizeof(Scalar))))
		{

		}
		~vectorContainer()
		{
			free(m_Data);
		}

		vectorContainer(const vectorContainer& rhs) : vectorContainer()
		{
			std::memcpy(m_Data, rhs.data(), _Size * sizeof(Scalar));
		}
		
		vectorContainer(const vectorContainer<Scalar, DYNAMIC_SIZE>& rhs) 
			: vectorContainer()
		{
			std::memcpy(m_Data, rhs.data(), _Size * sizeof(Scalar));
		}

		template<typename U>
		vectorContainer(const vectorContainer<U, _Size>& rhs)
		{
			size_t index = 0;
			for (; index < _Size; index += vmath_template_core::unrollStride_fix<_Size>)
			{
				vmath_template_core::fowardUnroll
					<vmath_template_core::unrollStride_fix<_Size>>
				(
					[&](size_t i)
					{
						m_Data[index + i] = static_cast<Scalar>(rhs[index + i]);
					}
				);
			}
			if constexpr (_Size % vmath_template_core::unrollStride_fix<_Size>)
			{
				for (; index < _Size; ++index)
				{
					m_Data[index] = static_cast<Scalar>(rhs[index]);
				}
			}

			return *this;
		}

		template<typename U>
		vectorContainer(const vectorContainer<U, DYNAMIC_SIZE>& rhs)
		{
			size_t index = 0;
			for (; index < _Size; index += static_cast<size_t>(8))
			{
				vmath_template_core::fowardUnroll<8>
				(
					[&](size_t i)
					{
						m_Data[index + i] = static_cast<Scalar>(rhs[index + i]);
					}
				);
			}
			if constexpr (_Size % static_cast<size_t>(8))
			{
				for (; index < _Size; ++index)
				{
					m_Data[index] = static_cast<Scalar>(rhs[index]);
				}
			}

			return *this;
		}

		vectorContainer& operator=(const vectorContainer& rhs)
		{
			if (this != &rhs)
			{
				std::memcpy(m_Data, rhs.data(), _Size * sizeof(Scalar));
			}
			return *this;
		}

		template<typename U>
		vectorContainer& operator=(const vectorContainer<U, DYNAMIC_SIZE>& rhs)
		{
			size_t index = 0;
			for (; index < _Size; index += static_cast<size_t>(8))
			{
				vmath_template_core::fowardUnroll<8>
				(
					[&](size_t i)
					{
						m_Data[index + i] = static_cast<Scalar>(rhs[index + i]);
					}
				);
			}
			if constexpr (_Size % static_cast<size_t>(8))
			{
				for (; index < _Size; ++index)
				{
					m_Data[index] = static_cast<Scalar>(rhs[index]);
				}
			}
			return *this;
		}

		VMATH_CONSTEXPR size_t size() const
		{
			return _Size;
		}

		VMATH_INLINE Scalar* data()
		{
			return m_Data;
		}

		VMATH_INLINE const Scalar* data() const
		{
			return m_Data;
		}

		VMATH_INLINE Scalar& operator[](size_t index)
		{
			return m_Data[index];
		}

		VMATH_INLINE const Scalar& operator[](size_t index) const
		{
			return m_Data[index];
		}

	};

	template<typename T>
	struct vectorContainer<T, DYNAMIC_SIZE>
	{
		typedef T Scalar;
		Scalar* m_Data = nullptr;
		size_t m_Size = 0;
		size_t m_Capacity = 0;

		vectorContainer(size_t size) : 
			m_Size(size), m_Capacity(size), 
			m_Data(static_cast<Scalar*>(malloc(size * sizeof(Scalar))))
		{

		}

		~vectorContainer()
		{
			free(m_Data);
		}

		vectorContainer(const vectorContainer& rhs) : 
			m_Size(rhs.m_Size), m_Capacity(rhs.m_Capacity), 
			m_Data(static_cast<Scalar*>(malloc(rhs.m_Capacity * sizeof(Scalar))))
		{
			std::memcpy(m_Data, rhs.data(), m_Size * sizeof(Scalar));
		}
		
		template<size_t Size>
			requires (Size != DYNAMIC_SIZE)
		vectorContainer(const vectorContainer<Scalar, Size>& rhs) :
				m_Size(Size), m_Capacity(Size),
				m_Data(static_cast<Scalar*>(malloc(Size * sizeof(Scalar))))
		{
			std::memcpy(m_Data, rhs.data(), Size * sizeof(Scalar));
		}

		template<typename U, size_t Size>
		vectorContainer(const vectorContainer<U, Size>& rhs) : 
				m_Size(Size), m_Capacity(Size),
				m_Data(static_cast<Scalar*>(malloc(Size * sizeof(Scalar))))
		{
			size_t index = 0;
			for (; index < Size; index += vmath_template_core::unrollStride_fix<Size>)
			{
				vmath_template_core::fowardUnroll
					<vmath_template_core::unrollStride_fix<Size>>
				(
					[&](size_t i)
					{
						m_Data[index + i] = static_cast<Scalar>(rhs[index + i]);
					}
				);
			}
			if constexpr (Size % vmath_template_core::unrollStride_fix<Size>)
			{
				for (; index < Size; ++index)
				{
					m_Data[index] = static_cast<Scalar>(rhs[index]);
				}
			}

			return *this;
		}

		template<typename U>
		vectorContainer(const vectorContainer<U, DYNAMIC_SIZE>& rhs) :
			m_Size(rhs.m_Size), m_Capacity(rhs.m_Capacity),
			m_Data(static_cast<Scalar*>(malloc(rhs.m_Capacity * sizeof(Scalar))))
		{
			size_t index = 0;
			if (m_Size >= static_cast<size_t>(8)) 
			{
				for (;index < m_Size; index += static_cast<size_t>(8))
				{
					vmath_template_core::fowardUnroll<8>
					(
						[&](size_t i)
						{
							m_Data[index + i] = static_cast<Scalar>(rhs[index + i]);
						}
					);
				}
			}
			for (; index < m_Size; ++index)
			{
				m_Data[index] = static_cast<Scalar>(rhs[index]);
			}

			return *this;
		}

		vectorContainer& operator=(const vectorContainer& rhs)
		{
			if (this != &rhs)
			{
				m_Size = rhs.size();
				if (m_Capacity < m_Size)
				{
					m_Capacity = m_Size << 1;
					Scalar* newData = static_cast<Scalar*>(malloc(m_Capacity * sizeof(Scalar)));
					std::swap(m_Data, newData);
					free(newData);
				}
				std::memcpy(m_Data, rhs.data(),  m_Size * sizeof(Scalar));
			}
			return *this;
		}

		template<size_t _Size>
			requires (_Size != DYNAMIC_SIZE)
		vectorContainer& operator=(const vectorContainer<Scalar, _Size>& rhs)
		{
			m_Size = _Size;
			if (m_Capacity < m_Size)
			{
				m_Capacity = m_Size;
				Scalar* newData = static_cast<Scalar*>(malloc(m_Capacity * sizeof(Scalar)));
				std::swap(m_Data, newData);
				free(newData);
			}
			std::memcpy(m_Data, rhs.data(), m_Size * sizeof(Scalar));
			return *this;
		}

		template<typename U, size_t _Size>
			requires (!std::is_same_v<T, U> && _Size != DYNAMIC_SIZE)
		vectorContainer& operator=(const vectorContainer<U, _Size>& rhs)
		{
			m_Size = _Size;
			if (m_Capacity < m_Size)
			{
				m_Capacity = m_Size;
				Scalar* newData = static_cast<Scalar*>(malloc(m_Capacity * sizeof(Scalar)));
				std::swap(m_Data, newData);
				free(newData);
			}

			size_t index = 0;
			for (; index < _Size; index += vmath_template_core::unrollStride_fix<_Size>)
			{
				vmath_template_core::fowardUnroll
					<vmath_template_core::unrollStride_fix<_Size>>
				(
					[&](size_t i)
					{
						m_Data[index + i] = static_cast<Scalar>(rhs[index + i]);
					}
				);
			}
			if constexpr (_Size % vmath_template_core::unrollStride_fix<_Size>)
			{
				for (; index < _Size; ++index)
				{
					m_Data[index] = static_cast<Scalar>(rhs[index]);
				}
			}
			return *this;
		}

		template<typename U>
		vectorContainer& operator=(const vectorContainer<U, DYNAMIC_SIZE>& rhs)
		{
			m_Size = rhs.size();
			if (m_Capacity < m_Size)
			{
				m_Capacity = m_Size << 1;
				Scalar* newData = static_cast<Scalar*>(malloc(m_Capacity * sizeof(Scalar)));
				std::swap(m_Data, newData);
				free(newData);
			}
			size_t index = 0;
			if(m_Size >= static_cast<size_t>(8))
			{ 
				for (; index < m_Size; index += static_cast<size_t>(8))
				{
					vmath_template_core::fowardUnroll<8>
					(
						[&](size_t i)
						{
							m_Data[index + i] = static_cast<Scalar>(rhs[index + i]);
						}
					);
				}
			}
			for (; index < m_Size; ++index)
			{
				m_Data[index] = static_cast<Scalar>(rhs[index]);
			}

			return *this;
		}

		VMATH_CONSTEXPR size_t size() const
		{
			return m_Size;
		}

		VMATH_INLINE Scalar* data()
		{
			return m_Data;
		}

		VMATH_INLINE const Scalar* data() const
		{
			return m_Data;
		}

		VMATH_INLINE Scalar& operator[](size_t index)
		{
			return m_Data[index];
		}

		VMATH_INLINE const Scalar& operator[](size_t index) const
		{
			return m_Data[index];
		}


	};


	template<typename T, size_t Size> 
	struct vectorData
	{
		vectorContainer<T, Size> m_Vector;

		vectorData()
		{

		}


		VMATH_INLINE T* data()
		{
			return m_Vector.m_Data;
		}

		VMATH_INLINE const T* data() const
		{
			return m_Vector.m_Data;
		}
		
		VMATH_CONSTEXPR size_t size() const
		{
			return Size;
		}

		VMATH_INLINE VMATH_CONSTEXPR T& operator[](size_t index)
		{
			return m_Vector.m_Data[index];
		}

		VMATH_INLINE VMATH_CONSTEXPR const T& operator[](size_t index) const
		{
			return m_Vector.m_Data[index];
		}
	};


}


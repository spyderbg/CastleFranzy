<script setup lang="ts">
const p = defineProps<{
  title: string;
  disabled?: boolean;
  active?: boolean;
}>();
const emit = defineEmits<{ click: [] }>();
</script>
<template>
  <button
    :class="['tool', { active: p.active }]"
    :title="p.title"
    :aria-label="p.title"
    :disabled="p.disabled"
    @click="emit('click')"
  >
    <slot />
  </button>
</template>
